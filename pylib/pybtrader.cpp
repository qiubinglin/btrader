#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "broker/pyservice/pydata_service.h"
#include "broker/pyservice/pytrade_service.h"

#include "py_journal.h"

PYBIND11_MODULE(pybtrader, m) {
    /* to do: Algorithm module for backtest. */

    /* broker */
    {
        auto submodule = m.def_submodule("broker");
        pybind11::class_<btra::broker::PyDataService>(submodule, "DataService");
        pybind11::class_<btra::broker::PyTradeService>(submodule, "TradeService");
    }

    /* tool */
    {
        auto submodule = m.def_submodule("tool");
        pybind11::class_<btra::PyJournalComm>(submodule, "JournalComm")
            .def("init", &btra::PyJournalComm::init)
            .def("start", &btra::PyJournalComm::start)
            .def("read", &btra::PyJournalComm::read)
            .def("write", &btra::PyJournalComm::write);
    }
}