#pragma once

#include "infra/common.h"

namespace btra {

struct GlobalState {
    bool is_multi_process{false};
    bool is_logger_setup{false};
};
DECLARE_SPTR(GlobalState)

}