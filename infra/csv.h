#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <optional>

namespace infra {

/**
 * @brief CSV Writer class for writing data to CSV files
 */
class CSVWriter {
public:
    /**
     * @brief Constructor
     * @param filename Output CSV file path
     * @param delimiter Field delimiter (default: comma)
     * @param quote_char Quote character for fields (default: double quote)
     * @param buffer_size Buffer size for writing (default: 64KB)
     */
    explicit CSVWriter(const std::string& filename, char delimiter = ',', char quote_char = '"', size_t buffer_size = 65536);
    
    /**
     * @brief Destructor
     */
    ~CSVWriter();
    
    /**
     * @brief Write header row
     * @param headers Vector of header strings
     * @return true if successful, false otherwise
     */
    bool write_header(const std::vector<std::string>& headers);
    
    /**
     * @brief Write a single row of data
     * @param row Vector of field strings
     * @return true if successful, false otherwise
     */
    bool write_row(const std::vector<std::string>& row);
    
    /**
     * @brief Write multiple rows at once
     * @param rows Vector of row vectors
     * @return true if successful, false otherwise
     */
    bool write_rows(const std::vector<std::vector<std::string>>& rows);
    
    /**
     * @brief Write row using string_view for better performance
     * @param row Vector of string_view
     * @return true if successful, false otherwise
     */
    bool write_row_sv(const std::vector<std::string_view>& row);
    
    /**
     * @brief Check if file is open and writable
     * @return true if file is open, false otherwise
     */
    bool is_open() const;
    
    /**
     * @brief Close the file
     */
    void close();
    
    /**
     * @brief Flush the output buffer
     */
    void flush();
    
    /**
     * @brief Set buffer size
     * @param size Buffer size in bytes
     */
    void set_buffer_size(size_t size);

private:
    std::string escape_field(const std::string& field) const;
    std::string escape_field_sv(std::string_view field) const;
    void write_field(std::string_view field);
    
    std::ofstream file_;
    std::string filename_;
    char delimiter_;
    char quote_char_;
    bool header_written_;
    size_t buffer_size_;
    std::string write_buffer_;
    size_t buffer_pos_;
};

/**
 * @brief CSV Reader class for reading data from CSV files
 */
class CSVReader {
public:
    /**
     * @brief Constructor
     * @param filename Input CSV file path
     * @param delimiter Field delimiter (default: comma)
     * @param quote_char Quote character for fields (default: double quote)
     * @param buffer_size Buffer size for reading (default: 64KB)
     */
    explicit CSVReader(const std::string& filename, char delimiter = ',', char quote_char = '"', size_t buffer_size = 65536);
    
    /**
     * @brief Destructor
     */
    ~CSVReader();
    
    /**
     * @brief Read and return header row
     * @return Vector of header strings, empty if failed
     */
    std::vector<std::string> read_header();
    
    /**
     * @brief Read next row from file
     * @return Vector of field strings, empty if EOF or error
     */
    std::vector<std::string> read_row();
    
    /**
     * @brief Read all remaining rows
     * @return Vector of row vectors
     */
    std::vector<std::vector<std::string>> read_all_rows();
    
    /**
     * @brief Read row with pre-allocated vector for better performance
     * @param row Pre-allocated vector to store the row
     * @return true if row was read, false if EOF or error
     */
    bool read_row_into(std::vector<std::string>& row);
    
    /**
     * @brief Check if file is open and readable
     * @return true if file is open, false otherwise
     */
    bool is_open() const;
    
    /**
     * @brief Check if end of file reached
     * @return true if EOF, false otherwise
     */
    bool eof() const;
    
    /**
     * @brief Close the file
     */
    void close();
    
    /**
     * @brief Reset file position to beginning
     */
    void reset();
    
    /**
     * @brief Set buffer size
     * @param size Buffer size in bytes
     */
    void set_buffer_size(size_t size);
    
    /**
     * @brief Get estimated row count (approximate)
     * @return Estimated number of rows
     */
    size_t estimate_row_count() const;

private:
    std::string unescape_field(const std::string& field) const;
    std::vector<std::string> parse_line(const std::string& line) const;
    bool parse_line_into(const std::string& line, std::vector<std::string>& fields) const;
    void reserve_fields_capacity(std::vector<std::string>& fields) const;
    
    std::ifstream file_;
    std::string filename_;
    char delimiter_;
    char quote_char_;
    std::vector<std::string> headers_;
    bool header_read_;
    size_t buffer_size_;
    size_t estimated_rows_;
    size_t file_size_;
};

// Smart pointer declarations
using CSVWriterSPtr = std::shared_ptr<CSVWriter>;
using CSVReaderSPtr = std::shared_ptr<CSVReader>;

} // namespace infra
