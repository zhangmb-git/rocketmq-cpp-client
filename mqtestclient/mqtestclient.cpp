// mqtestclient.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "RocketMQ.h"
#include "Consumer.h"
#include "Producer.h"

int main() {

    //Consumer  consumer;
    //consumer.Start();
    Producer  producer;
    std::string strBody = "{\"shopId\":\"1510\",\"itemId\":\"81213\",\"userId\":\"1002\",\"timeStamp\":\"1610436162000\"}";

    for (int i = 0; i < 2; i++) {
        producer.Send(strTopic, strTag, strBody);
    }

    return 0;
}


