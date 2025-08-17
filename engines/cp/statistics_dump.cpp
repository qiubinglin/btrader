#include "statistics_dump.h"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "core/enums.h"
#include "infra/time.h"

namespace btra {

StatisticsDump::~StatisticsDump() {
    if (kline_dump_stream_.is_open()) {
        kline_dump_stream_.flush();
        kline_dump_stream_.close();
    }
    if (asset_dump_stream_.is_open()) {
        asset_dump_stream_.flush();
        asset_dump_stream_.close();
    }
    if (trade_dump_stream_.is_open()) {
        trade_dump_stream_.flush();
        trade_dump_stream_.close();
    }
}

void StatisticsDump::init(const std::string& outdir) {
    try {
        // Create output directory if it doesn't exist
        std::filesystem::create_directories(outdir);

        // Initialize file streams with proper file paths
        std::string kline_file = outdir + "/kline_data.csv";
        std::string asset_file = outdir + "/asset_data.csv";
        std::string trade_file = outdir + "/trade_data.csv";

        // Open file streams
        kline_dump_stream_.open(kline_file, std::ios::out | std::ios::trunc);
        asset_dump_stream_.open(asset_file, std::ios::out | std::ios::trunc);
        trade_dump_stream_.open(trade_file, std::ios::out | std::ios::trunc);

        // Check if files opened successfully
        if (!kline_dump_stream_.is_open()) {
            throw std::runtime_error("Failed to open kline data file: " + kline_file);
        }
        if (!asset_dump_stream_.is_open()) {
            throw std::runtime_error("Failed to open asset data file: " + asset_file);
        }
        if (!trade_dump_stream_.is_open()) {
            throw std::runtime_error("Failed to open trade data file: " + trade_file);
        }

        // Write CSV headers
        write_csv_headers();

        printf("StatisticsDump initialized successfully. Output directory: %s\n", outdir.c_str());
        printf("Files created:\n");
        printf("  - Kline data: %s\n", kline_file.c_str());
        printf("  - Asset data: %s\n", asset_file.c_str());
        printf("  - Trade data: %s\n", trade_file.c_str());

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
        if (trade_dump_stream_.is_open()) {
            trade_dump_stream_.flush();
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

        asset_dump_stream_.flush();

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

        kline_dump_stream_.flush();

    } catch (const std::exception& e) {
        printf("Error logging kline data: %s\n", e.what());
    }
}

void StatisticsDump::log_trade(const Trade& trade) {
    try {
        if (!trade_dump_stream_.is_open()) {
            printf("Warning: Trade dump stream not open\n");
            return;
        }

        // Convert timestamp to readable format
        std::string time_str = format_timestamp(trade.trade_time);

        // Get side string
        std::string side_str = (trade.side == enums::Side::Buy) ? "BUY" : "SELL";

        // Get offset string
        std::string offset_str;
        switch (trade.offset) {
            case enums::Offset::Open:
                offset_str = "OPEN";
                break;
            case enums::Offset::Close:
                offset_str = "CLOSE";
                break;
            case enums::Offset::CloseToday:
                offset_str = "CLOSE_TODAY";
                break;
            case enums::Offset::CloseYesterday:
                offset_str = "CLOSE_YESTERDAY";
                break;
            default:
                offset_str = "UNKNOWN";
                break;
        }

        // Write trade data in CSV format
        trade_dump_stream_ << time_str << "," << trade.instrument_id.to_string() << "," << trade.exchange_id.to_string()
                           << "," << side_str << "," << offset_str << "," << std::fixed << std::setprecision(6)
                           << trade.price << "," << trade.volume << "," << std::fixed << std::setprecision(6)
                           << trade.commission << "," << std::fixed << std::setprecision(6) << trade.tax << "\n";

        trade_dump_stream_.flush();

    } catch (const std::exception& e) {
        printf("Error logging trade data: %s\n", e.what());
    }
}

// Helper methods implementation
void StatisticsDump::write_csv_headers() {
    try {
        // Write kline data headers
        kline_dump_stream_ << "start_time,end_time,open,high,low,close,volume,start_volume,tick_count\n";

        // Write asset data headers
        asset_dump_stream_ << "timestamp,asset_value\n";

        // Write trade data headers
        trade_dump_stream_ << "timestamp,instrument_id,exchange_id,side,offset,price,volume,commission,tax\n";

        // Flush headers immediately
        kline_dump_stream_.flush();
        asset_dump_stream_.flush();
        trade_dump_stream_.flush();

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