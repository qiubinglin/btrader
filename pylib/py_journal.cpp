#include "py_journal.h"

namespace btra {

void PyJournalReader::init(const std::string &conf) {
    auto j = Json::json::parse(conf);
    readpack_.init(j);
}

pybind11::dict PyJournalReader::read() {
    pybind11::dict res;
    /* frame to dict */
    return res;
}

void PyJournalWriter::init(const std::string &conf) {
    auto j = Json::json::parse(conf);
    writepack_.init(j);
}

void PyJournalWriter::write(const pybind11::dict &data) {
    /* dict to frame */
}

} // namespace btra