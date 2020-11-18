#include "MsgListener.h"
#include <stdio.h>
#include "Consumer.h"

ConsumeStatus MsgListener::consumeMessage(const std::vector<MQMessageExt>& msgs) {

    if (!m_pConsumer) {
        printf("consume null ptr");
        return  RECONSUME_LATER;
    }

    return  m_pConsumer->ConsumeMessage(msgs);
}


