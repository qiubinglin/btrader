#include "infra/log.h"

#include "spdlog/sinks/rotating_file_sink.h"
#include <filesystem>
#include <iostream>
#include <spdlog/sinks/basic_file_sink.h>

namespace infra {

void LogMgr::set_level(LogLevel level) { spdlog::set_level(level); }

void LogMgr::set_pattern(const std::string &pattern) { spdlog::set_pattern(pattern); }

void LogMgr::setup_basic_log(const std::string &path) {
    try {
        auto abs_path = std::filesystem::absolute(path);
        auto dir = abs_path.parent_path();
        if (not std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
        auto logger = spdlog::basic_logger_mt("btra", abs_path.string());
        spdlog::set_default_logger(logger);
    } catch (const spdlog::spdlog_ex &ex) {
        std::cerr << "setup_basic_log failed: " << ex.what() << std::endl;
    }
}

void LogMgr::setup_rotating_log(const std::string &path, size_t max_size, size_t max_files) {
    try {
        auto abs_path = std::filesystem::absolute(path);
        auto dir = abs_path.parent_path();
        if (not std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
        auto logger = spdlog::rotating_logger_mt("btra", abs_path.string(), max_size, max_files);
        spdlog::set_default_logger(logger);
    } catch (const spdlog::spdlog_ex &ex) {
        std::cerr << "setup_rotating_log failed: " << ex.what() << std::endl;
    }
}

void LogMgr::shutdown() { spdlog::shutdown(); }

} // namespace infra