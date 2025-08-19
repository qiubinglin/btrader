#include "filedataservice.h"

#include "infra/log.h"

namespace btra::broker {

void FileDataService::setup(const Json::json &cfg) {
    filename_ = cfg["account"].get<std::string>();

    // Create CSV reader with optimized buffer size for backtesting
    reader_ = std::make_unique<infra::CSVReader>(filename_, ',', '"', 1024 * 1024); // 1MB buffer

    if (!reader_->is_open()) {
        INFRA_LOG_ERROR("Failed to open CSV file: {}", filename_);
        throw std::runtime_error("Failed to open CSV file: " + filename_);
    }

    INFRA_LOG_INFO("CSV file opened successfully: {}", filename_);
}

void FileDataService::start() {
    if (!reader_ || !reader_->is_open()) {
        INFRA_LOG_ERROR("CSV reader is not properly initialized");
        return;
    }

    try {
        // Read header first
        auto headers = reader_->read_header();
        if (headers.empty()) {
            INFRA_LOG_ERROR("Failed to read CSV header or file is empty");
            return;
        }

        INFRA_LOG_INFO("CSV headers: {}", headers.size());

        // Pre-allocate row vector for better performance
        std::vector<std::string> row;
        row.reserve(headers.size());

        size_t row_count = 0;
        bool header_skipped = false;

        // Read all rows
        while (reader_->read_row_into(row)) {
            if (!header_skipped) {
                // Skip header row if it wasn't properly handled
                header_skipped = true;
                continue;
            }

            if (row.size() < 6) {
                INFRA_LOG_WARN("Row {} has insufficient columns: expected 6, got {}", row_count, row.size());
                continue;
            }

            try {
                Bar bar;

                // Parse bar data from CSV row
                // Expected format: date, close, high, low, open, volume
                // Column indices: 0=date, 1=close, 2=high, 3=low, 4=open, 5=volume
                bar.close = std::stod(row[1]);
                bar.high = std::stod(row[2]);
                bar.low = std::stod(row[3]);
                bar.open = std::stod(row[4]);
                bar.volume = std::stol(row[5]);

                // Parse date and set time range
                infra::time::strptimerange(row[0].c_str(), HISTORY_DAY_FORMAT, bar.start_time, bar.end_time);

                // Validate that the time parsing was successful
                if (bar.start_time <= 0 || bar.end_time <= 0) {
                    INFRA_LOG_WARN("Failed to parse date or invalid time range: {}", row[0]);
                    continue;
                }

                // Set default values for required fields
                bar.instrument_id = "BACKTEST";                      // Default instrument ID for backtesting
                bar.exchange_id = "BACKTEST";                        // Default exchange ID for backtesting
                bar.instrument_type = enums::InstrumentType::Future; // Default type
                bar.tick_count = 1;                                  // Default tick count
                bar.start_volume = 0;                                // Default start volume

                // Write bar data to writer
                if (writer_) {
                    writer_->write(infra::time::now_time(), bar);
                    row_count++;

                    if (row_count % 1000 == 0) {
                        INFRA_LOG_INFO("Processed {} bars", row_count);
                    }
                } else {
                    INFRA_LOG_ERROR("Writer is not available");
                    break;
                }

            } catch (const std::exception &e) {
                INFRA_LOG_ERROR("Error parsing row {}: {}", row_count, e.what());
                continue;
            }
        }

        INFRA_LOG_INFO("Backtesting completed. Total bars processed: {}", row_count);

        // Send termination signal
        // if (writer_) {
        //     writer_->write(infra::time::now_time(), Termination());
        // }

    } catch (const std::exception &e) {
        INFRA_LOG_ERROR("Error during backtesting: {}", e.what());
    }
}

void FileDataService::stop() {
    if (reader_) {
        reader_->close();
        INFRA_LOG_INFO("CSV reader closed");
    }
}

bool FileDataService::subscribe(const std::vector<InstrumentKey> &instrument_keys) {
    INFRA_LOG_INFO("Backtesting CSV data service - subscription not applicable, returning true");
    return true;
}

bool FileDataService::unsubscribe(const std::vector<InstrumentKey> &instrument_keys) {
    INFRA_LOG_INFO("Backtesting CSV data service - unsubscription not applicable, returning true");
    return true;
}

} // namespace btra::broker