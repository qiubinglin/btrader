#pragma once

#include <pybind11/stl.h>

#include "core/journal_pack.h"

namespace btra {

class PyJournalReader {
public:
    PyJournalReader() {}
    void init(const std::string &conf);
    pybind11::dict read();

private:
    JourReadPack readpack_;
};

class PyJournalWriter {
public:
    PyJournalWriter() {}
    void init(const std::string &conf);
    void write(const pybind11::dict &data);

private:
    JourWritePack writepack_;
};

} // namespace btra