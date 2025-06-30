#include "cp/backtest_dump.h"

#include <sstream>

#include "infra/csv.h"
#include "infra/time.h"

namespace btra {

void BacktestDump::dump_files(const std::string &dir) const {
    // Implementation for dumping the data to a directory
    // This is a placeholder; actual implementation would depend on the file format and requirements
    std::string asset_file = dir + "/asset.csv";
    infra::CSVWriter file_writer;
    file_writer.set_file(asset_file);
    std::vector<std::string> row;
    for (const auto &val : asset_values) {
        // std::cout << val.time << " " << val.value << std::endl;
        row.clear();
        row.push_back(infra::time::strftime(val.time));
        row.push_back(std::to_string(val.value));
        file_writer.write_row(row);
    }
    file_writer.finish();
}

} // namespace btra