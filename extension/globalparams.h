#pragma once

#include "infra/singleton.h"

namespace btra {

struct StatisticsParams {
    enum Mode {
        None,
        All,
    };
    Mode stats_mode;

    static Mode str2mode(const std::string &str) {
        if (str == "all") {
            return Mode::All;
        }
        return Mode::None;
    }

    bool stats_all() const {
        return stats_mode == Mode::All;
    }

    bool active() const {
        return stats_mode != Mode::None;
    }

    void set_mode(const std::string &str) {
        stats_mode = StatisticsParams::str2mode(str);
    }
};

/**
 * @brief Hold some parameters in main configuration `system` for convenience
 * 
 */
struct GlobalParams {
    std::string root_dir; /* data output root directory */

    StatisticsParams stat_params;

    bool is_simulation{false};

    bool is_backtest{false};
};

} // namespace btra

template class Singleton<btra::GlobalParams>;