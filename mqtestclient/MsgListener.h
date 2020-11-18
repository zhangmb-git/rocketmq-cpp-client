#pragma once
#include "MQMessageListener.h"
#include "Global.h"
using namespace rocketmq;

class Consumer;
class MsgListener : public MessageListenerConcurrently {
  public:
    MsgListener(Consumer* pConsumer): m_pConsumer(pConsumer) {}
    virtual ~MsgListener() {}
    virtual ConsumeStatus consumeMessage(const std::vector<MQMessageExt>& msgs);

  private:
    Consumer*  m_pConsumer;
};
