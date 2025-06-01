#pragma once

#include <spdlog/spdlog.h>

#define INFRA_LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)

#define INFRA_LOG_INFO(...) spdlog::info(__VA_ARGS__)

#define INFRA_LOG_WARN(...) spdlog::warn(__VA_ARGS__)

#define INFRA_LOG_ERROR(...) spdlog::error(__VA_ARGS__)

#define INFRA_LOG_CRITICAL(...) spdlog::critical(__VA_ARGS__)

namespace infra {

using LogLevel = spdlog::level::level_enum;

class LogMgr {
public:
    static void set_level(LogLevel level);
    static void set_pattern(const std::string &pattern);
    static void setup_basic_log(const std::string &path);

    /**
     * @brief Set the up rotating log object
     * 
     * @param path 
     * @param max_size in unit MB
     * @param max_files 
     */
    static void setup_rotating_log(const std::string &path, size_t max_size, size_t max_files);

    static void shutdown();
};

} // namespace infra
