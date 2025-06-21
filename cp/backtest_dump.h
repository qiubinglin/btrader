#pragma once

#include <deque>
#include <vector>

#include "core/types.h"

namespace btra {

struct BacktestDump {
    struct KLine {
        int64_t start_time;
        int64_t end_time;
        double open;
        double close;
        double low;
        double high;
        double volume;
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

    std::deque<KLine> klines;
    std::deque<AssetValue> asset_values;
    std::vector<Trancation> trancations;

    void dump_file(const std::string &file_name) const;
};

} // namespace btra