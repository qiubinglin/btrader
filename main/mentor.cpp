#include "mentor.h"

#include <csignal>
#include <fstream>

#include "cp_engine.h"
#include "infra/log.h"
#include "infra/time.h"
#include "md_engine.h"
#include "option_parser.h"
#include "td_engine.h"

void signal_handler(int signal) {
    INFRA_LOG_ERROR("Caught signal {}, flushing logs...", signal);
    infra::LogMgr::shutdown(); // flush all loggers and shutdown
    /* Use _Exit to skip the execution of atexit-registered cleanup routines, which may otherwise cause issues during
     * the destruction of global or static resources.*/

    if (!btra::Mentor::s_socket_path.empty()) {
        std::filesystem::remove(btra::Mentor::s_socket_path);
        btra::Mentor::s_socket_path.clear();
    }
    std::_Exit(0);
}

namespace btra {

std::string Mentor::s_socket_path = "";

Mentor::Mentor(int argc, char **argv) {
    std::signal(SIGINT, signal_handler);  // Ctrl+C
    std::signal(SIGTERM, signal_handler); // kill
    if (argc <= 1) {
        throw std::runtime_error("argc <= 1");
    }
    std::string role, cfg_file;
    OptionParser parser;
    parser.option(0, "role", 1, [&](const char *s) { role = s; });
    parser.option(0, "cfg", 1, [&](const char *s) { cfg_file = s; });
    parser.parse(argv);
    GlobalStateSPtr global_state = std::make_shared<GlobalState>();
    init(role, cfg_file, global_state);
}

Mentor::Mentor() {}

Mentor::~Mentor() {
    if (event_engine_) {
        delete event_engine_;
    }
    if (!s_socket_path.empty()) {
        std::filesystem::remove(s_socket_path);
        s_socket_path.clear();
    }
}

void Mentor::init(const std::string &role, const std::string &cfg_file, GlobalStateSPtr global_state) {
    role_ = role;
    cfg_file_ = cfg_file;
    global_state_ = global_state;
    _init();
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

void Mentor::_init() {
    setup(role_);
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

void Mentor::setup(const std::string &id) {
    std::ifstream f(cfg_file_);
    auto cfg = Json::json::parse(f);

    /* Remove existing output directory */
    if (id == "master") {
        std::string output_dir = cfg["system"]["output_root_path"].get<std::string>();
        auto output_path = std::filesystem::absolute(output_dir);
        if (std::filesystem::exists(output_path)) {
            std::cout << fmt::format("Output directory {} already exists, remove it? y/[n]: ", output_path.string());
            std::string str;
            if (std::getline(std::cin, str)) {
                if (str == "y" || str == "Y") {
                    std::filesystem::remove_all(output_path);
                } else {
                    std::cout << "Output directory not removed, exiting..." << std::endl;
                    std::_Exit(0);
                }
            }
        }
    }

    /* time */
    std::string time_unit_str = cfg["system"]["time_unit"].get<std::string>();
    if (time_unit_str == "nano") {
        infra::time::get_instance().unit = infra::TimeUnit::NANO;
    } else if (time_unit_str == "milli") {
        infra::time::get_instance().unit = infra::TimeUnit::MILLI;
    } else if (time_unit_str == "sec") {
        infra::time::get_instance().unit = infra::TimeUnit::SEC;
    } else {
        throw std::runtime_error("No such time unit!");
    }

    /* log */
    if (not global_state_->is_logger_setup) {
        global_state_->is_logger_setup = true;
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
        if (cfg["system"]["log"]["mode"].get<std::string>() != "stdout") {
            std::string log_file = path + id + ".log";
            if (std::filesystem::exists(log_file)) {
                std::filesystem::remove_all(log_file);
            }
            std::string logger = cfg["system"]["log"]["logger"].get<std::string>();
            if (logger == "basic") {
                infra::LogMgr::setup_basic_log(log_file);
            } else if (logger == "rotating") {
                size_t max_size = cfg["system"]["log"]["max_size"].get<size_t>();
                size_t max_files = cfg["system"]["log"]["max_files"].get<size_t>();
                infra::LogMgr::setup_rotating_log(log_file, max_size * MB, max_files);
            } else {
                throw std::runtime_error("No such logger type!");
            }
        }
        infra::LogMgr::set_level(level);
    }
}

} // namespace btra