#include "mentor.h"

#include <csignal>
#include <fstream>

#include "cp_engine.h"
#include "infra/log.h"
#include "md_engine.h"
#include "option_parser.h"
#include "td_engine.h"

void signal_handler(int signal) {
    INFRA_LOG_ERROR("Caught signal {}, flushing logs...", signal);
    infra::LogMgr::shutdown(); // flush all loggers and shutdown
    /* Use _Exit to skip the execution of atexit-registered cleanup routines, which may otherwise cause issues during
     * the destruction of global or static resources.*/
    std::_Exit(0);
}

namespace btra {

Mentor::Mentor(int argc, char **argv) {
    std::signal(SIGINT, signal_handler);  // Ctrl+C
    std::signal(SIGTERM, signal_handler); // kill
    if (argc <= 1) {
        throw std::runtime_error("argc <= 1");
    }
    OptionParser parser;
    parser.option(0, "role", 1, [&](const char *s) { role_ = s; });
    parser.option(0, "cfg", 1, [&](const char *s) { cfg_file_ = s; });
    parser.parse(argv);
    init();
}

Mentor::~Mentor() {
    if (event_engine_) {
        delete event_engine_;
    }
}

int Mentor::run() {
    if (role_ == "master") {
        return _run();
    }
    if (event_engine_) {
        event_engine_->run();
        return 0;
    } else {
        throw std::runtime_error("Event engine is not created!!!");
        return -1;
    }
}

void Mentor::init() {
    setup_log(role_);
    INFRA_LOG_CRITICAL("{} start!", role_);
    if (role_ == "master") {
        return;
    }
    if (role_ == "cp") {
        event_engine_ = new CPEngine();
    } else if (role_ == "md") {
        event_engine_ = new MDEngine();
    } else if (role_ == "td") {
        event_engine_ = new TDEngine();
    } else {
        throw std::runtime_error("Not supported role!");
    }

    std::ifstream f(cfg_file_);
    event_engine_->setcfg(Json::json::parse(f));
}

void Mentor::setup_log(const std::string &id) {
    std::ifstream f(cfg_file_);
    auto cfg = Json::json::parse(f);
    std::string level_str = cfg["system"]["log"]["level"].get<std::string>();
    std::string path = cfg["system"]["log"]["path"].get<std::string>();
    if (path.back() != '/') {
        path.append("/");
    }
    infra::LogLevel level = infra::LogLevel::err;
    if (level_str == "debug") {
        level = infra::LogLevel::debug;
    } else if (level_str == "info") {
        level = infra::LogLevel::info;
    } else if (level_str == "warn") {
        level = infra::LogLevel::warn;
    } else if (level_str == "error") {
        level = infra::LogLevel::err;
    } else if (level_str == "critical") {
        level = infra::LogLevel::critical;
    } else {
        throw std::runtime_error("No such log level!");
    }
    infra::LogMgr::setup_basic_log(path + id + ".log");
    infra::LogMgr::set_level(level);
}

} // namespace btra