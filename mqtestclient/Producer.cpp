#include "Producer.h"
#include "Global.h"

Producer::Producer(): m_producer(strGroupName) {
    _init();
}

Producer::~Producer() {

}

void Producer::_init() {
    m_producer.setNamesrvAddr(strAdd);
    m_producer.setGroupName(strGroupName);
    m_producer.setRetryTimes(3);
    //m_producer.setNamesrvDomain(info.namesrv_domain);
    m_producer.start();
}


void Producer::Send(const string topic, const string tag, const string body) {
    MQMessage msg(topic,  tag, body);

    try {
        m_producer.send(msg);

    } catch (MQException& e) {
        std::cout << e << endl;  // if catch excepiton , need re-send this msg by
        // service
    }

}


void Producer::Stop() {
    m_producer.shutdown();
}

