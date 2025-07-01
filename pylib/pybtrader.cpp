#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "broker/pyservice/pydata_service.h"
#include "broker/pyservice/pytrade_service.h"

PYBIND11_MODULE(pybtrader, m) {
    /* to do: Algorithm module for backtest. */

    /* broker */
    {
        auto submodule = m.def_submodule("broker");
        pybind11::class_<btra::broker::PyDataService>(submodule, "DataService");
        pybind11::class_<btra::broker::PyTradeService>(submodule, "TradeService");
    }
}