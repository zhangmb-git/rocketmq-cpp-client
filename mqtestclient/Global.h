#pragma once
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <iomanip>
#include <iostream>

using  namespace  std;

const  string  strAdd = "10.0.57.216:9876;10.0.58.152:9876";
const  string  strGroupName = "ITEM_GROUP";
const  string  strTopic = "ITEM";
const  string  strTag = "ITEM_UPDATE";

class TpsReportService {
  public:
    TpsReportService();
    ~TpsReportService();

    void Start();
    void Increment();
    void TpsReport();

  private:
    std::chrono::seconds tps_interval_;
    std::shared_ptr<std::thread> tps_thread_;
    std::atomic<bool> quit_flag_;
    std::atomic<long> tps_count_;
};


//



