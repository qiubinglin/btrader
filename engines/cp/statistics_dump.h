#pragma once

#include <deque>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <sstream>

#include "core/types.h"

namespace btra {

/**
 * @brief Only support one instrument for now
 * 
 */
struct StatisticsDump {
    ~StatisticsDump();
    void init(const std::string &outdir);
    void flush();
    void log_asset(int64_t time, double asset);
    void log_kline(const Bar &kline);
    void log_trade(const Trade &trade);

private:
    // Helper methods
    void write_csv_headers();
    std::string format_timestamp(int64_t timestamp);
    
    // File streams
    std::ofstream kline_dump_stream_;
    std::ofstream asset_dump_stream_;
    std::ofstream trade_dump_stream_;
};

} // namespace btra