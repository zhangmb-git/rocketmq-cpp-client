#include "RocketMQ.h"
#include "MQMessageQueue.h"
#include "PullResult.h"
#include "base/util/string_util.h"

//void  CPriceUpdateTask::run() {
//    PriceUpdateMsg* pMsg = (PriceUpdateMsg*)m_param;
//
//    if (pMsg == nullptr) return;
//
//    //查询redis  遍历结果，然后
//
//    //
//
//}
//
//
//CRocketMQ::CRocketMQ(): m_bConsumerFlag(true), m_bProducerFlag(false) {
//    m_pool.Init(2);
//}
//
//CRocketMQ::~CRocketMQ() {
//
//    if (m_bConsumerFlag) {
//        m_consumer.Stop();
//    }
//
//    if (m_bProducerFlag) {
//
//        m_producer.Stop();
//    }
//}
//
//void  CRocketMQ::PushMsg(std::string sellerID, std::string itemID) {
//    PriceUpdateMsg*  pMsg = new PriceUpdateMsg;
//    pMsg->itemID = itemID;
//    pMsg->userID = sellerID;
//
//    CPriceUpdateTask* pTask = new CPriceUpdateTask();
//    pTask->SetParam(pMsg);
//    m_pool.AddTask(pTask);
//    return;
//}
//



