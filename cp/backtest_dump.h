#pragma once

#include <deque>
#include <vector>

#include "core/types.h"

namespace btra {

/**
 * @brief Only support one instrument for now
 * 
 */
struct BacktestDump {
    struct KLine {
        int64_t start_time;
        int64_t end_time;
        double open;
        double close;
        double low;
        double high;
        int64_t volume;
    };

    struct AssetValue {
        int64_t time;
        double value;
    };

    struct Trancation {
        int64_t time;
        infra::Array<char, INSTRUMENT_ID_LEN> symbol;
        double price;
        double volume;
        enums::Side side;
    };

    std::deque<AssetValue> asset_values;
    std::deque<Trancation> trancations;

    void dump_files(const std::string &dir) const;
};

} // namespace btra