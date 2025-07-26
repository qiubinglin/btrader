#pragma once

#include <pybind11/stl.h>

#include "core/journal_comm_data.h"

namespace btra {

class PyJournalComm {
public:
    PyJournalComm() {}
    void init(const std::string &conf_file);
    void start();
    pybind11::dict read();
    void write(const pybind11::dict &data);

private:
    JourCommData comm_data_;
};

} // namespace btra