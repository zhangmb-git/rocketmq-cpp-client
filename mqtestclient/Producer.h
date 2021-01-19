#pragma once
#include "DefaultMQProducer.h"

using namespace  rocketmq;
using namespace std;

class Producer {
  public:
    Producer();
    ~Producer();

    void Send(const string topic, const string tag, const string body);
    //void Send(const string topic, const string tag, const string strKey, const string body);

    void  Stop();
  private:
    void _init();

  private:
    DefaultMQProducer m_producer;
};

