// mqtestclient.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "RocketMQ.h"
#include "Consumer.h"

int main() {

    Consumer  consumer;
    consumer.Start();

    return 0;
}


