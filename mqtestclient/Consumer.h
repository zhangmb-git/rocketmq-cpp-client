#pragma once

#include <vector>
#include <atomic>
#include <mutex>
#include "Consumer.h"
#include "MQMessageListener.h"
#include "DefaultMQPushConsumer.h"
#include "MsgListener.h"

using namespace rocketmq;


//const  long long llset = 26;


//Ïû·ÑÕß
class Consumer {
  public:
    Consumer();
    ~Consumer();

  public:

    void Start();
    void Stop();
    ConsumeStatus ConsumeMessage(const std::vector<MQMessageExt>& msgs);

  private:
    void _init();

  private:
    std::mutex m_mtx;
    std::condition_variable m_finished;
    TpsReportService m_tps;
    std::atomic<int> m_msgCount;

    DefaultMQPushConsumer consumer;
    MsgListener  m_listener;



};


