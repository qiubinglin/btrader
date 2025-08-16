#pragma once

#include "infra/singleton.h"

namespace btra {

struct StatisticsParams {
    enum Mode {
        None,
        All,
    };
    Mode stat_mode;

    static Mode str2mode(const std::string &str) {
        if (str == "all") {
            return Mode::All;
        }
        return Mode::None;
    }

    bool stat_all() const {
        return stat_mode == Mode::All;
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
};

} // namespace btra

template class Singleton<btra::GlobalParams>;