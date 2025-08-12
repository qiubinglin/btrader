#pragma once

#include "infra/singleton.h"

namespace btra {

struct GlobalParams {
    std::string root_dir;
};

} // namespace btra

template class Singleton<btra::GlobalParams>;