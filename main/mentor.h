#pragma once

#include <thread>

#include "eventengine.h"
#include "global_state.h"

namespace btra {

class Mentor {
public:
    static std::string s_socket_path;

    /**
     * @brief The entry for multi-process.
     *
     * @param argc
     * @param argv
     */
    Mentor(int argc, char **argv);

    /**
     * @brief The entry for multi-thread.
     *
     */
    Mentor();

    ~Mentor();

    void init(const std::string &role, const std::string &cfg_file, GlobalStateSPtr global_state);
    int run();

private:
    void _init();
    void setup(const std::string &id);

    int _run();

    void send_eventfds(const std::vector<int> &eventfd_list, const std::string &socket_path);

    std::string role_;
    std::string cfg_file_;
    GlobalStateSPtr global_state_;
    EventEngine *event_engine_ = nullptr;

    std::unique_ptr<std::thread> send_eventfds_th_;
};

} // namespace btra