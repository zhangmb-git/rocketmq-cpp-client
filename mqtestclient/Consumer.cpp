#include "Consumer.h"

Consumer::Consumer() : consumer(strGroupName), m_listener(this), m_msgCount(1) {
    _init();
}

Consumer::~Consumer() {
    Stop();
}

void  Consumer::_init() {
    consumer.setNamesrvAddr(strAdd);
    consumer.setGroupName(strGroupName);
    //consumer.setNamesrvDomain(info.namesrv_domain);
    consumer.setConsumeFromWhere(CONSUME_FROM_LAST_OFFSET);
    consumer.setInstanceName(strGroupName);
    consumer.subscribe(strTopic, strTag);

    consumer.setConsumeThreadCount(15);
    consumer.setConsumeMessageBatchMaxSize(100);

    consumer.setTcpTransportTryLockTimeout(1000);
    consumer.setTcpTransportConnectTimeout(400);

    //设置消费模式，CLUSTERING-集群模式，BROADCASTING-广播模式
    consumer.setMessageModel(CLUSTERING);
    // 非阻塞模式，拉取超时时间，默认10s
    //consumer.setConsumerPullTimeoutMillis(4000);
    // 长轮询模式，Consumer连接在Broker挂起最长时间，默认20s
    // consumer.setBrokerSuspendMaxTimeMillis(3000);
    // 长轮询模式，拉取超时时间，默认30s
    //consumer.setConsumerTimeoutMillisWhenSuspend(5000);


    consumer.registerMessageListener(&m_listener);

}

void Consumer::Start() {
    try {
        consumer.start();

    } catch (MQClientException& e) {
        cout << e << endl;
    }

    m_tps.Start();
    int msgcount = m_msgCount.load();
}

void Consumer::Stop() {

    {
        std::unique_lock<std::mutex> lck(m_mtx);
        m_finished.wait(lck);
    }
    consumer.shutdown();
}

ConsumeStatus Consumer::ConsumeMessage(const std::vector<MQMessageExt>& msgs) {
    m_msgCount.store(m_msgCount.load() - msgs.size());

    for (size_t i = 0; i < msgs.size(); ++i) {
        m_tps.Increment();
    }

    if (m_msgCount.load() <= 0) {
        std::unique_lock<std::mutex> lck(m_mtx);
        m_finished.notify_one();
    }

    return  CONSUME_SUCCESS;
}
