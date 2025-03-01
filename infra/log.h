#pragma once

#include <iostream>

#include "infra/format.h"

#define INFRA_LOG_DEBUG(...) std::cout << fmt::format(__VA_ARGS__) << std::endl

#define INFRA_LOG_INFO(...)

#define INFRA_LOG_WARN(...)

#define INFRA_LOG_ERROR(...)
