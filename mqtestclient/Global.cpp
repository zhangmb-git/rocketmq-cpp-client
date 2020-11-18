#include "Global.h"

TpsReportService::TpsReportService() : tps_interval_(1), quit_flag_(false), tps_count_(0) {
}

TpsReportService::~TpsReportService() {
    quit_flag_.store(true);

    if (tps_thread_ == nullptr) {
        std::cout << "tps_thread_ is null" << std::endl;
        return;
    }

    if (tps_thread_->joinable()) {
        tps_thread_->join();
    }
}

void  TpsReportService::Start() {
    if (tps_thread_ != nullptr) {
        std::cout << "tps_thread_ is not null" << std::endl;
        return;
    }

    tps_thread_.reset(new std::thread(std::bind(&TpsReportService::TpsReport, this)));
}

void TpsReportService::Increment() {
    ++tps_count_;
}

void TpsReportService::TpsReport() {
    while (!quit_flag_.load()) {
        std::this_thread::sleep_for(tps_interval_);
        std::cout << "tps: " << tps_count_.load() << std::endl;
        tps_count_.store(0);
    }
}