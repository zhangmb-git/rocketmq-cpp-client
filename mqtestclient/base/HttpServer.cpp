/** @file HttpServer.h
  * @brief 
  * @author yingchun.xu
  * @date 2019-09-03
  */

#include "HttpServer.h"
#include "netlib.h"

typedef hash_map<uint32_t, HttpServer *> HttpServerMap_t;

static HttpServerMap_t g_http_server_map;
static rb_timer_item g_http_conn_timer;
// conn_handle 从0开始递增，可以防止因socket handle重用引起的一些冲突
static uint32_t g_conn_handle_generator = 0;

HttpServer *FindHttpServerByHandle(uint32_t handle) {
    HttpServer *pConn = NULL;
    HttpServerMap_t::iterator it = g_http_server_map.find(handle);
    if (it != g_http_server_map.end()) {
        pConn = it->second;
    }
    return pConn;
}

void http_conn_callback(void *callback_data, uint8_t msg, uint32_t handle, uint32_t uParam, void *pParam) {
    NOTUSED_ARG(uParam);
    NOTUSED_ARG(pParam);

    // convert void* to uint32_t, oops
    uint32_t conn_handle = *((uint32_t *) (&callback_data));
    HttpServer *pConn = FindHttpServerByHandle(conn_handle);
    if (!pConn) {
        return;
    }

    switch (msg) {
        case NETLIB_MSG_READ:
            pConn->OnRead();
            break;
        case NETLIB_MSG_WRITE:
            pConn->OnWrite();
            break;
        case NETLIB_MSG_CLOSE:
            pConn->OnClose();
            break;
        default:
            ERROR("!!!httpconn_callback error msg: %d ", msg);
            break;
    }
}

void http_conn_timer_callback(void *callback_data, uint8_t msg, uint32_t handle, void *pParam) {
    (void) callback_data;
    (void) msg;
    (void) handle;
    (void) pParam;

    HttpServer *pConn = NULL;
    HttpServerMap_t::iterator it, it_old;
    uint64_t cur_time = get_tick_count();

    for (it = g_http_server_map.begin(); it != g_http_server_map.end();) {
        it_old = it;
        it++;

        pConn = it_old->second;
        pConn->OnTimer(cur_time);
    }
}

void init_http_server() {
    rbtimer_init(&g_http_conn_timer, http_conn_timer_callback, NULL, 1000, 0, 0);
    netlib_register_timer(&g_http_conn_timer);
}


HttpServer::HttpServer() {
    m_busy = false;
    m_sock_handle = NETLIB_INVALID_HANDLE;
    m_state = CONN_STATE_IDLE;

    m_last_send_tick = m_last_recv_tick = get_tick_count();
    m_conn_handle = ++g_conn_handle_generator;
    if (m_conn_handle == 0) {
        m_conn_handle = ++g_conn_handle_generator;
    }
}

HttpServer::~HttpServer() {

}

void HttpServer::OnConnect(net_handle_t handle) {
    m_sock_handle = handle;
    m_state = CONN_STATE_CONNECTED;
    g_http_server_map.insert(make_pair(m_conn_handle, this));

    netlib_option(handle, NETLIB_OPT_SET_CALLBACK, (void *) http_conn_callback);
    netlib_option(handle, NETLIB_OPT_SET_CALLBACK_DATA, reinterpret_cast<void *>(m_conn_handle));
    netlib_option(handle, NETLIB_OPT_GET_REMOTE_IP, (void *) &m_peer_ip);
    netlib_option(handle, NETLIB_OPT_GET_REMOTE_PORT, (void *) &m_peer_port);

    DEBUG("OnConnect, handle=%d\n,ip=%s,port=%d", handle, m_peer_ip.c_str(), m_peer_port);
}

int HttpServer::Send(void *data, int len) {
    m_last_send_tick = get_tick_count();

    if (m_busy) {
        m_out_buf.Write(data, len);
        return len;
    }

    int ret = netlib_send(m_sock_handle, data, len);
    if (ret < 0)
        ret = 0;

    if (ret < len) {
        m_out_buf.Write((char *) data + ret, len - ret);
        m_busy = true;
        log("not send all, remain=%d\n", m_out_buf.GetWriteOffset());
    } else {
        OnWriteComplete();
    }
    return len;
}

void HttpServer::Close() {
    m_state = CONN_STATE_CLOSED;

    g_http_server_map.erase(m_conn_handle);
    netlib_close(m_sock_handle);

    ReleaseRef();
}

void HttpServer::OnRead() {
    log("for http conn, remote ip:%s, remote port:%d", GetPeerIP(), m_peer_port);
    for (;;) {
        uint32_t free_buf_len = m_in_buf.GetAllocSize() - m_in_buf.GetWriteOffset();
        if (free_buf_len < READ_BUF_SIZE + 1)
            m_in_buf.Extend(READ_BUF_SIZE + 1);

        int ret = netlib_recv(m_sock_handle, m_in_buf.GetBuffer() + m_in_buf.GetWriteOffset(), READ_BUF_SIZE);
        if (ret <= 0)
            break;

        m_in_buf.IncWriteOffset(ret);

        m_last_recv_tick = get_tick_count();
    }

    // 每次请求对应一个HTTP连接，所以读完数据后，不用在同一个连接里面准备读取下个请求
    char *in_buf = (char *) m_in_buf.GetBuffer();
    uint32_t buf_len = m_in_buf.GetWriteOffset();
    in_buf[buf_len] = '\0';

    // 如果buf_len 过长可能是受到攻击，则断开连接
    // 正常的url最大长度为2048，我们接受的所有数据长度不得大于1K
    if (buf_len > 1024) {
        loge("get too much data:%d ", buf_len);
        Close();
        return;
    }

    m_cHttpParser.ParseHttpContent(in_buf, buf_len);

    if (m_cHttpParser.IsReadAll()) {
        log("get original url:%s", m_cHttpParser.GetUrl());
        string url = _urlDecode(m_cHttpParser.GetUrl());
        if (url == "") {
            loge("illegal request url format");
            Close();
            return;
        }
        log("parsed url:%s", url.c_str());

        // get path
        // http://zhaogang.com/check.txt
        std::string path = "/";
        size_t pos_path = url.find_last_of("/");
        if (pos_path == std::string::npos) {
            log("invalid url, url=%s ", url.c_str());
            Close();
        }

        // get parameter
        size_t pos = url.find('?');
        if (pos != std::string::npos) {
            path = url.substr(pos_path, pos);

            size_t equal_pos = 0, ampersand_pos = 0;
            string name, value;
            string params = url.substr(pos + 1);
            pos = 0;
            while (pos < params.length()) {
                equal_pos = params.find('=', pos);
                if (equal_pos == std::string::npos) {
                    loge("invalid parameter name:%s", params.c_str());
                    Close();
                }
                name = params.substr(pos, equal_pos - pos);
                pos = equal_pos + 1;
                ampersand_pos = params.find('&', pos);
                if (ampersand_pos == std::string::npos) {
                    // to the end
                    value = params.substr(pos);
                    pos = params.length();
                } else {
                    value = params.substr(pos, ampersand_pos - pos);
                    pos = ampersand_pos + 1;
                }
                mParams.insert(make_pair(name, value));
            }
        } else {
            path = url.substr(pos_path);
        }

        bool ret = HandleUrl(url, path, mParams);
        if (!ret) {
            log("url unknown, url=%s ", url.c_str());
            Close();
        }
    }
}

void HttpServer::OnWrite() {
    if (!m_busy)
        return;

    int ret = netlib_send(m_sock_handle, m_out_buf.GetBuffer(), m_out_buf.GetWriteOffset());
    if (ret < 0)
        ret = 0;

    int out_buf_size = (int) m_out_buf.GetWriteOffset();

    m_out_buf.Read(NULL, ret);

    if (ret < out_buf_size) {
        m_busy = true;
        log("not send all, remain=%d ", m_out_buf.GetWriteOffset());
    } else {
        OnWriteComplete();
        m_busy = false;
    }
}

void HttpServer::OnClose() {
    Close();
}

void HttpServer::OnTimer(uint64_t curr_tick) {
    if (curr_tick > m_last_recv_tick + HTTP_CONN_TIMEOUT) {
        log("HttpConn timeout, handle=%d ", m_conn_handle);
        Close();
    }
}

void HttpServer::OnWriteComplete() {
    //log("write complete ");
    Close();
}


bool HttpServer::HandleUrl(std::string full_url, std::string path, ParamMap_t mParams) {
    log("full_url=%s,path=%s", full_url.c_str(), path.c_str());
    return false;
}

unsigned char HttpServer::_fromHex(unsigned char x) {
    unsigned char y;
    if (x >= 'A' && x <= 'F') y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'f') y = x - 'a' + 10;
    else if (x >= '0' && x <= '9') y = x - '0';
        //else assert(0);
    else y = 255;
    return y;
}

std::string HttpServer::_urlDecode(const std::string &str) {
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++) {
        if (str[i] == '+') strTemp += ' ';
        else if (str[i] == '%') {
            //assert(i + 2 < length);
            if (i + 2 >= length) {
                return "";
            }
            unsigned char high = _fromHex((unsigned char) str[++i]);
            if (high == 255) {
                return "";
            }
            unsigned char low = _fromHex((unsigned char) str[++i]);
            if (low == 255) {
                return "";
            }
            strTemp += high * 16 + low;
        } else strTemp += str[i];
    }
    return strTemp;
}
