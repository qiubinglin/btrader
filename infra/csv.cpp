#include "infra/csv.h"
/* Above main header */

#include <sstream>

namespace infra {

static void split(const std::string &str, char delimiter, std::vector<std::string> &out) {
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        out.push_back(token);
    }
}

CSVReader::~CSVReader() {}

bool CSVReader::set_file(const std::string &filename) {
    if (ifs_.is_open()) {
        ifs_.close();
    }
    ifs_.open(filename);
    return ifs_.is_open();
}

void CSVReader::read_row(std::vector<std::string> &out) {
    out.clear();
    std::string line;
    if (std::getline(ifs_, line)) {
        split(line, ',', out);
    }
}

bool CSVReader::is_eof() const { return ifs_.eof(); }

bool CSVWriter::set_file(const std::string &filename) {
    if (ofs_.is_open()) {
        ofs_.close();
    }
    ofs_.open(filename);
    return ofs_.is_open();
}

void CSVWriter::write_row(const std::vector<std::string> &row) {
    char delimiter = ',';
    for (size_t i = 0; i < row.size(); ++i) {
        if (i != 0) {
            ofs_ << delimiter;
        }
        ofs_ << row[i];
    }
    ofs_ << '\n';
}

void CSVWriter::finish() {
    ofs_.flush();
    ofs_.close();
}

} // namespace infra