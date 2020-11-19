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

    //��������ģʽ��CLUSTERING-��Ⱥģʽ��BROADCASTING-�㲥ģʽ
    consumer.setMessageModel(CLUSTERING);
    // ������ģʽ����ȡ��ʱʱ�䣬Ĭ��10s
    //consumer.setConsumerPullTimeoutMillis(4000);
    // ����ѯģʽ��Consumer������Broker�����ʱ�䣬Ĭ��20s
    // consumer.setBrokerSuspendMaxTimeMillis(3000);
    // ����ѯģʽ����ȡ��ʱʱ�䣬Ĭ��30s
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
