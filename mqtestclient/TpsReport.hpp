#pragma once

class TpsReportService {
  public:
    TpsReportService() : tps_interval_(1), quit_flag_(false), tps_count_(0) {}

    ~TpsReportService() {
        quit_flag_.store(true);

        if (tps_thread_ == nullptr) {
            std::cout << "tps_thread_ is null" << std::endl;
            return;
        }

        if (tps_thread_->joinable()) {
            tps_thread_->join();
        }
    }

    void start() {
        if (tps_thread_ != nullptr) {
            std::cout << "tps_thread_ is not null" << std::endl;
            return;
        }

        tps_thread_.reset(new std::thread(std::bind(&TpsReportService::TpsReport, this)));
    }

    void Increment() {
        ++tps_count_;
    }

    void TpsReport() {
        while (!quit_flag_.load()) {
            std::this_thread::sleep_for(tps_interval_);
            std::cout << "tps: " << tps_count_.load() << std::endl;
            tps_count_.store(0);
        }
    }

  private:
    std::chrono::seconds tps_interval_;
    std::shared_ptr<std::thread> tps_thread_;
    std::atomic<bool> quit_flag_;
    std::atomic<long> tps_count_;
};