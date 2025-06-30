#pragma once

#include <fstream>
#include <string>
#include <vector>

namespace infra {

class CSVReader {
public:
    ~CSVReader();
    bool set_file(const std::string &filename);

    void read_row(std::vector<std::string> &out);
    bool is_eof() const;

private:
    std::ifstream ifs_;
};

class CSVWriter {
public:
    bool set_file(const std::string &filename);

    void write_row(const std::vector<std::string> &row);
    void finish();

private:
    std::ofstream ofs_;
};

} // namespace infra