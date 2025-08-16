#include "statistics_dump.h"

#include <ctime>
#include <iostream>
#include <sstream>

#include "infra/time.h"

namespace btra {

StatisticsDump::~StatisticsDump() {
    if (kline_dump_stream_.is_open()) {
        kline_dump_stream_.close();
    }
    if (asset_dump_stream_.is_open()) {
        asset_dump_stream_.close();
    }
    if (transaction_dump_stream_.is_open()) {
        transaction_dump_stream_.close();
    }
}

void StatisticsDump::init(const std::string& outdir) {
    try {
        // Create output directory if it doesn't exist
        std::filesystem::create_directories(outdir);

        // Initialize file streams with proper file paths
        std::string kline_file = outdir + "/kline_data.csv";
        std::string asset_file = outdir + "/asset_data.csv";
        std::string transaction_file = outdir + "/transaction_data.csv";

        // Open file streams
        kline_dump_stream_.open(kline_file, std::ios::out | std::ios::trunc);
        asset_dump_stream_.open(asset_file, std::ios::out | std::ios::trunc);
        transaction_dump_stream_.open(transaction_file, std::ios::out | std::ios::trunc);

        // Check if files opened successfully
        if (!kline_dump_stream_.is_open()) {
            throw std::runtime_error("Failed to open kline data file: " + kline_file);
        }
        if (!asset_dump_stream_.is_open()) {
            throw std::runtime_error("Failed to open asset data file: " + asset_file);
        }
        if (!transaction_dump_stream_.is_open()) {
            throw std::runtime_error("Failed to open transaction data file: " + transaction_file);
        }

        // Write CSV headers
        write_csv_headers();

        printf("StatisticsDump initialized successfully. Output directory: %s\n", outdir.c_str());
        printf("Files created:\n");
        printf("  - Kline data: %s\n", kline_file.c_str());
        printf("  - Asset data: %s\n", asset_file.c_str());
        printf("  - Transaction data: %s\n", transaction_file.c_str());

    } catch (const std::exception& e) {
        printf("Error initializing StatisticsDump: %s\n", e.what());
        throw;
    }
}

void StatisticsDump::flush() {
    try {
        if (kline_dump_stream_.is_open()) {
            kline_dump_stream_.flush();
        }
        if (asset_dump_stream_.is_open()) {
            asset_dump_stream_.flush();
        }
        if (transaction_dump_stream_.is_open()) {
            transaction_dump_stream_.flush();
        }
    } catch (const std::exception& e) {
        printf("Error flushing StatisticsDump streams: %s\n", e.what());
    }
}

void StatisticsDump::log_asset(int64_t time, double asset) {
    try {
        if (!asset_dump_stream_.is_open()) {
            printf("Warning: Asset dump stream not open\n");
            return;
        }

        // Convert timestamp to readable format
        std::string time_str = format_timestamp(time);

        // Write asset data in CSV format: timestamp,asset_value
        asset_dump_stream_ << time_str << "," << std::fixed << std::setprecision(6) << asset << "\n";

        // Auto-flush every 1000 records to ensure data is written
        static int record_count = 0;
        if (++record_count % 1000 == 0) {
            asset_dump_stream_.flush();
        }

    } catch (const std::exception& e) {
        printf("Error logging asset data: %s\n", e.what());
    }
}

void StatisticsDump::log_kline(const Bar& kline) {
    try {
        if (!kline_dump_stream_.is_open()) {
            printf("Warning: Kline dump stream not open\n");
            return;
        }

        // Convert timestamps to readable format
        std::string start_time_str = format_timestamp(kline.start_time);
        std::string end_time_str = format_timestamp(kline.end_time);

        // Write kline data in CSV format
        kline_dump_stream_ << start_time_str << "," << end_time_str << "," << std::fixed << std::setprecision(6)
                           << kline.open << "," << std::fixed << std::setprecision(6) << kline.high << "," << std::fixed
                           << std::setprecision(6) << kline.low << "," << std::fixed << std::setprecision(6)
                           << kline.close << "," << kline.volume << "," << kline.start_volume << "," << kline.tick_count
                           << "\n";

        // Auto-flush every 1000 records
        static int record_count = 0;
        if (++record_count % 1000 == 0) {
            kline_dump_stream_.flush();
        }

    } catch (const std::exception& e) {
        printf("Error logging kline data: %s\n", e.what());
    }
}

void StatisticsDump::log_transaction(const Transaction& transaction) {
    try {
        if (!transaction_dump_stream_.is_open()) {
            printf("Warning: Transaction dump stream not open\n");
            return;
        }

        // Convert timestamp to readable format
        std::string time_str = format_timestamp(transaction.data_time);

        // Get side string
        std::string side_str = (transaction.side == enums::Side::Buy) ? "BUY" : "SELL";

        // Write transaction data in CSV format
        transaction_dump_stream_ << time_str << "," << transaction.instrument_id.to_string() << ","
                                 << transaction.exchange_id.to_string() << "," << side_str << "," << std::fixed
                                 << std::setprecision(6) << transaction.price << "," << transaction.volume << "\n";

        // Auto-flush every 1000 records
        static int record_count = 0;
        if (++record_count % 1000 == 0) {
            transaction_dump_stream_.flush();
        }

    } catch (const std::exception& e) {
        printf("Error logging transaction data: %s\n", e.what());
    }
}

// Helper methods implementation
void StatisticsDump::write_csv_headers() {
    try {
        // Write kline data headers
        kline_dump_stream_ << "start_time,end_time,open,high,low,close,volume,start_volume,tick_count\n";

        // Write asset data headers
        asset_dump_stream_ << "timestamp,asset_value\n";

        // Write transaction data headers
        transaction_dump_stream_ << "timestamp,instrument_id,exchange_id,side,price,volume\n";

        // Flush headers immediately
        kline_dump_stream_.flush();
        asset_dump_stream_.flush();
        transaction_dump_stream_.flush();

    } catch (const std::exception& e) {
        printf("Error writing CSV headers: %s\n", e.what());
    }
}

std::string StatisticsDump::format_timestamp(int64_t timestamp) {
    try {
        // Convert Unix timestamp (milliseconds) to readable format
        if (timestamp <= 0) {
            return "1970-01-01 00:00:00";
        }

        // Convert milliseconds to seconds if needed
        time_t time_seconds = timestamp / 1000;

        // Convert to local time
        struct tm* timeinfo = localtime(&time_seconds);
        if (!timeinfo) {
            return "Invalid_Time";
        }

        // Format as YYYY-MM-DD HH:MM:SS
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(4) << (timeinfo->tm_year + 1900) << "-" << std::setfill('0')
            << std::setw(2) << (timeinfo->tm_mon + 1) << "-" << std::setfill('0') << std::setw(2) << timeinfo->tm_mday
            << " " << std::setfill('0') << std::setw(2) << timeinfo->tm_hour << ":" << std::setfill('0') << std::setw(2)
            << timeinfo->tm_min << ":" << std::setfill('0') << std::setw(2) << timeinfo->tm_sec;

        return oss.str();

    } catch (const std::exception& e) {
        printf("Error formatting timestamp: %s\n", e.what());
        return "Error_Time";
    }
}

} // namespace btra