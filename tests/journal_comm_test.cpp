#include "pylib/py_journal.h"

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>

#include "infra/log.h"

int main() {
    infra::LogMgr::set_level(infra::LogLevel::debug);
    pybind11::scoped_interpreter guard{};

    std::string conf_file = "/home/qiubinglin/btrader/usrconf/jourcomm_main_cfg.tpl.json";
    btra::PyJournalComm jour_comm;
    jour_comm.init(conf_file);
    jour_comm.start();
    while (true) {
        jour_comm.read();
    }
    return 0;
}