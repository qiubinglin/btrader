#include "signalhandler.h"

#include "infra/log.h"
#include "infra/singleton.h"

namespace infra {

void handle_sig(int signum) {
    INFRA_LOG_ERROR("Caught signal {}, flushing logs...", signum);
    INSTANCE(SignalHandler).exec_sig_cbs(signum);
}

SignalHandler::SignalHandler() {
    std::signal(SIGINT, handle_sig);  // Ctrl+C
    std::signal(SIGTERM, handle_sig); // kill
}

SignalHandler::~SignalHandler() {}

void SignalHandler::register_to_filo(int signum, std::function<void()> cb) { sig_cbs[signum].push_front(cb); }

void SignalHandler::exec_sig_cbs(int signum) {
    for (auto &cb : sig_cbs[signum]) {
        cb();
    }
}

} // namespace infra