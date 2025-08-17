#pragma once

#include <csignal>
#include <functional>
#include <list>
#include <map>

#include "infra/singleton.h"

namespace infra {

class SignalHandler {
    SignalHandler();
    ~SignalHandler();

public:
    /**
     * @brief Register cb to FILO. The fist register, the last execute
     *
     * @param signum
     * @param cb
     */
    void register_to_filo(int signum, std::function<void()> cb);

    void exec_sig_cbs(int signum);

private:
    std::map<int, std::list<std::function<void()>>> sig_cbs;

    friend class Singleton<SignalHandler>;
};

} // namespace infra

template class Singleton<infra::SignalHandler>;