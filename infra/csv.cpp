#include "csv.h"
#include "log.h"
#include <sstream>
#include <algorithm>
#include <cstring>

namespace infra {

// CSVWriter implementation
CSVWriter::CSVWriter(const std::string& filename, char delimiter, char quote_char, size_t buffer_size)
    : filename_(filename)
    , delimiter_(delimiter)
    , quote_char_(quote_char)
    , header_written_(false)
    , buffer_size_(buffer_size)
    , write_buffer_()
    , buffer_pos_(0) {
    
    file_.open(filename);
    if (!file_.is_open()) {
        INFRA_LOG_ERROR("Failed to open file for writing: {}", filename);
    } else {
        // Set buffer size for better performance
        file_.rdbuf()->pubsetbuf(nullptr, buffer_size_);
        write_buffer_.reserve(buffer_size_);
    }
}

CSVWriter::~CSVWriter() {
    close();
}

bool CSVWriter::write_header(const std::vector<std::string>& headers) {
    if (!is_open()) {
        INFRA_LOG_ERROR("File is not open for writing: {}", filename_);
        return false;
    }
    
    if (header_written_) {
        INFRA_LOG_WARN("Header already written to file: {}", filename_);
        return false;
    }
    
    bool success = write_row(headers);
    if (success) {
        header_written_ = true;
    }
    return success;
}

bool CSVWriter::write_row(const std::vector<std::string>& row) {
    if (!is_open()) {
        INFRA_LOG_ERROR("File is not open for writing: {}", filename_);
        return false;
    }
    
    try {
        for (size_t i = 0; i < row.size(); ++i) {
            if (i > 0) {
                write_field(std::string_view(&delimiter_, 1));
            }
            write_field(row[i]);
        }
        write_field(std::string_view("\n", 1));
        
        if (!file_.good()) {
            INFRA_LOG_ERROR("Error writing to file: {}", filename_);
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        INFRA_LOG_ERROR("Exception while writing row: {}", e.what());
        return false;
    }
}

bool CSVWriter::write_row_sv(const std::vector<std::string_view>& row) {
    if (!is_open()) {
        INFRA_LOG_ERROR("File is not open for writing: {}", filename_);
        return false;
    }
    
    try {
        for (size_t i = 0; i < row.size(); ++i) {
            if (i > 0) {
                write_field(std::string_view(&delimiter_, 1));
            }
            write_field(row[i]);
        }
        write_field(std::string_view("\n", 1));
        
        if (!file_.good()) {
            INFRA_LOG_ERROR("Error writing to file: {}", filename_);
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        INFRA_LOG_ERROR("Exception while writing row: {}", e.what());
        return false;
    }
}

bool CSVWriter::write_rows(const std::vector<std::vector<std::string>>& rows) {
    if (!is_open()) {
        INFRA_LOG_ERROR("File is not open for writing: {}", filename_);
        return false;
    }
    
    // Pre-allocate buffer for better performance
    if (rows.size() > 0 && rows[0].size() > 0) {
        size_t avg_row_size = 0;
        for (const auto& row : rows) {
            for (const auto& field : row) {
                avg_row_size += field.size() + 2; // +2 for delimiter and potential quotes
            }
            avg_row_size += 1; // newline
        }
        avg_row_size = avg_row_size / rows.size();
        write_buffer_.reserve(std::min(avg_row_size * 10, buffer_size_));
    }
    
    for (const auto& row : rows) {
        if (!write_row(row)) {
            return false;
        }
    }
    return true;
}

bool CSVWriter::is_open() const {
    return file_.is_open() && file_.good();
}

void CSVWriter::close() {
    flush();
    if (file_.is_open()) {
        file_.close();
    }
}

void CSVWriter::flush() {
    if (file_.is_open() && buffer_pos_ > 0) {
        file_.write(write_buffer_.data(), buffer_pos_);
        buffer_pos_ = 0;
        file_.flush();
    }
}

void CSVWriter::set_buffer_size(size_t size) {
    buffer_size_ = size;
    write_buffer_.reserve(buffer_size_);
    file_.rdbuf()->pubsetbuf(nullptr, buffer_size_);
}

void CSVWriter::write_field(std::string_view field) {
    // Check if we need to flush buffer
    if (buffer_pos_ + field.size() > buffer_size_) {
        flush();
    }
    
    // If field is small, add to buffer
    if (field.size() <= buffer_size_) {
        std::memcpy(&write_buffer_[buffer_pos_], field.data(), field.size());
        buffer_pos_ += field.size();
    } else {
        // For large fields, write directly
        flush();
        file_.write(field.data(), field.size());
    }
}

std::string CSVWriter::escape_field(const std::string& field) const {
    return escape_field_sv(field);
}

std::string CSVWriter::escape_field_sv(std::string_view field) const {
    // Fast path: check if field needs quoting using memchr for better performance
    bool needs_quotes = false;
    if (field.size() > 0) {
        needs_quotes = std::memchr(field.data(), delimiter_, field.size()) != nullptr ||
                      std::memchr(field.data(), quote_char_, field.size()) != nullptr ||
                      std::memchr(field.data(), '\n', field.size()) != nullptr ||
                      std::memchr(field.data(), '\r', field.size()) != nullptr;
    }
    
    if (!needs_quotes) {
        return std::string(field);
    }
    
    // Pre-allocate string with estimated size
    std::string escaped;
    escaped.reserve(field.size() + 2); // +2 for outer quotes
    
    // Count quotes to estimate final size
    size_t quote_count = 0;
    for (char c : field) {
        if (c == quote_char_) quote_count++;
    }
    escaped.reserve(field.size() + 2 + quote_count);
    
    escaped += quote_char_;
    for (char c : field) {
        if (c == quote_char_) {
            escaped += quote_char_;
        }
        escaped += c;
    }
    escaped += quote_char_;
    
    return escaped;
}

// CSVReader implementation
CSVReader::CSVReader(const std::string& filename, char delimiter, char quote_char, size_t buffer_size)
    : filename_(filename)
    , delimiter_(delimiter)
    , quote_char_(quote_char)
    , header_read_(false)
    , buffer_size_(buffer_size)
    , estimated_rows_(0)
    , file_size_(0) {
    
    file_.open(filename);
    if (!file_.is_open()) {
        INFRA_LOG_ERROR("Failed to open file for reading: {}", filename);
    } else {
        // Set buffer size for better performance
        file_.rdbuf()->pubsetbuf(nullptr, buffer_size_);
        
        // Get file size for estimation
        file_.seekg(0, std::ios::end);
        file_size_ = file_.tellg();
        file_.seekg(0, std::ios::beg);
        
        // Estimate rows based on file size (rough estimate)
        if (file_size_ > 0) {
            estimated_rows_ = file_size_ / 100; // Assume average row is ~100 bytes
        }
    }
}

CSVReader::~CSVReader() {
    close();
}

std::vector<std::string> CSVReader::read_header() {
    if (!is_open()) {
        INFRA_LOG_ERROR("File is not open for reading: {}", filename_);
        return {};
    }
    
    if (header_read_) {
        INFRA_LOG_WARN("Header already read from file: {}", filename_);
        return headers_;
    }
    
    std::string line;
    line.reserve(1024); // Pre-allocate for typical header size
    
    if (std::getline(file_, line)) {
        headers_ = parse_line(line);
        header_read_ = true;
        return headers_;
    }
    
    INFRA_LOG_ERROR("Failed to read header from file: {}", filename_);
    return {};
}

std::vector<std::string> CSVReader::read_row() {
    if (!is_open()) {
        INFRA_LOG_ERROR("File is not open for reading: {}", filename_);
        return {};
    }
    
    if (!header_read_) {
        INFRA_LOG_ERROR("Header must be read before reading rows: {}", filename_);
        return {};
    }
    
    std::vector<std::string> row;
    reserve_fields_capacity(row);
    
    if (read_row_into(row)) {
        return row;
    }
    
    return {};
}

std::vector<std::vector<std::string>> CSVReader::read_all_rows() {
    if (!is_open()) {
        INFRA_LOG_ERROR("File is not open for reading: {}", filename_);
        return {};
    }
    
    if (!header_read_) {
        INFRA_LOG_ERROR("Header must be read before reading rows: {}", filename_);
        return {};
    }
    
    std::vector<std::vector<std::string>> rows;
    if (estimated_rows_ > 0) {
        rows.reserve(estimated_rows_);
    }
    
    std::vector<std::string> row;
    reserve_fields_capacity(row);
    
    while (read_row_into(row)) {
        rows.push_back(std::move(row));
        reserve_fields_capacity(row);
    }
    
    return rows;
}

bool CSVReader::read_row_into(std::vector<std::string>& row) {
    if (!is_open()) {
        INFRA_LOG_ERROR("File is not open for reading: {}", filename_);
        return false;
    }
    
    if (!header_read_) {
        INFRA_LOG_ERROR("Header must be read before reading rows: {}", filename_);
        return false;
    }
    
    std::string line;
    line.reserve(1024); // Pre-allocate for typical row size
    
    if (std::getline(file_, line)) {
        row.clear();
        return parse_line_into(line, row);
    }
    
    return false;
}

bool CSVReader::is_open() const {
    return file_.is_open() && file_.good();
}

bool CSVReader::eof() const {
    return file_.eof();
}

void CSVReader::close() {
    if (file_.is_open()) {
        file_.close();
    }
}

void CSVReader::reset() {
    if (file_.is_open()) {
        file_.clear();
        file_.seekg(0);
        header_read_ = false;
        headers_.clear();
    }
}

void CSVReader::set_buffer_size(size_t size) {
    buffer_size_ = size;
    if (file_.is_open()) {
        file_.rdbuf()->pubsetbuf(nullptr, buffer_size_);
    }
}

size_t CSVReader::estimate_row_count() const {
    return estimated_rows_;
}

void CSVReader::reserve_fields_capacity(std::vector<std::string>& fields) const {
    if (!headers_.empty()) {
        fields.reserve(headers_.size());
    } else {
        fields.reserve(10); // Default capacity
    }
}

std::string CSVReader::unescape_field(const std::string& field) const {
    if (field.empty() || field[0] != quote_char_ || field.back() != quote_char_) {
        return field;
    }
    
    // Remove outer quotes
    std::string unescaped = field.substr(1, field.length() - 2);
    
    // Unescape doubled quotes - use memchr for better performance
    size_t pos = 0;
    while ((pos = unescaped.find(std::string(2, quote_char_), pos)) != std::string::npos) {
        unescaped.replace(pos, 2, 1, quote_char_);
        pos += 1;
    }
    
    return unescaped;
}

std::vector<std::string> CSVReader::parse_line(const std::string& line) const {
    std::vector<std::string> fields;
    reserve_fields_capacity(fields);
    parse_line_into(line, fields);
    return fields;
}

bool CSVReader::parse_line_into(const std::string& line, std::vector<std::string>& fields) const {
    fields.clear();
    
    if (line.empty()) {
        return false;
    }
    
    // Pre-allocate fields vector
    reserve_fields_capacity(fields);
    
    std::string current_field;
    current_field.reserve(256); // Pre-allocate for typical field size
    
    bool in_quotes = false;
    const char* data = line.data();
    const size_t length = line.length();
    
    for (size_t i = 0; i < length; ++i) {
        char c = data[i];
        
        if (c == quote_char_) {
            if (in_quotes && i + 1 < length && data[i + 1] == quote_char_) {
                // Double quote - escape sequence
                current_field += quote_char_;
                i++; // Skip next quote
            } else {
                // Toggle quote state
                in_quotes = !in_quotes;
            }
        } else if (c == delimiter_ && !in_quotes) {
            // End of field
            fields.push_back(unescape_field(current_field));
            current_field.clear();
            current_field.reserve(256); // Keep pre-allocated capacity
        } else {
            current_field += c;
        }
    }
    
    // Add the last field
    fields.push_back(unescape_field(current_field));
    
    return true;
}

} // namespace infra
