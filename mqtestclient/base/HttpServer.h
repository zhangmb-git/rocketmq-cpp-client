/** @file HttpServer.h
  * @brief 抽象HTTP_Server
  * @author yingchun.xu
  * @date 2019-09-03
  */

#ifndef BASE_HTTPSERVER_H_
#define BASE_HTTPSERVER_H_

#include "util.h"
#include "HttpParserWrapper.h"

#define HTTP_CONN_TIMEOUT            60000

#define READ_BUF_SIZE    2048
#define HTTP_RESPONSE_HTML          "HTTP/1.1 200 OK\r\n"\
                                    "Connection:close\r\n"\
                                    "Content-Length:%d\r\n"\
                                    "Content-Type:text/html;charset=utf-8\r\n\r\n%s"
#define HTTP_RESPONSE_HTML_MAX      1024

enum {
    CONN_STATE_IDLE,
    CONN_STATE_CONNECTED,
    CONN_STATE_OPEN,
    CONN_STATE_CLOSED,
};

typedef hash_map<string, string> ParamMap_t;

/** @class HttpServer
  * @brief
  */
class HttpServer : CRefObject {
public:
    HttpServer();

    ~HttpServer();

public:
    uint32_t GetConnHandle() { return m_conn_handle; }

    char *GetPeerIP() { return (char *) m_peer_ip.c_str(); }

    virtual int Send(void *data, int len);

    virtual void Close();

    virtual void OnConnect(net_handle_t handle);

    virtual void OnRead();

    virtual void OnWrite();

    virtual void OnClose();

    virtual void OnTimer(uint64_t curr_tick);

    virtual void OnWriteComplete();

protected:
    virtual bool HandleUrl(std::string full_url, std::string path, ParamMap_t mParams);

private:
    static unsigned char _fromHex(unsigned char x);

    static std::string _urlDecode(const std::string &str);

protected:
    net_handle_t m_sock_handle;
    uint32_t m_conn_handle;
    bool m_busy;

    uint32_t m_state;
    std::string m_peer_ip;
    uint16_t m_peer_port;
    CSimpleBuffer m_in_buf;
    CSimpleBuffer m_out_buf;

    uint64_t m_last_send_tick;
    uint64_t m_last_recv_tick;

    CHttpParserWrapper m_cHttpParser;
    ParamMap_t mParams;
};

HttpServer *FindHttpServerByHandle(uint32_t handle);

void init_http_server();

#endif //BASE_HTTPSERVER_H_
