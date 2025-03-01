#include "csv.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace infra {

bool CSV::read(const std::string& filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << filename << std::endl;
    return false;
  }

  data_.clear();
  std::string line;
  while (std::getline(file, line)) {
    std::vector<std::string> row = split(line, ',');
    data_.emplace_back(row);
  }
  file.close();
  return true;
}

bool CSV::write(const std::string& filename) const {
  std::ofstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << filename << std::endl;
    return false;
  }

  for (const auto& row : data_) {
    file << join(row, ',') << "\n";
  }
  file.close();
  return true;
}

const std::vector<std::string>& CSV::get_row(size_t index) const {
  if (index >= data_.size()) {
    throw std::out_of_range("Row index out of range");
  }
  return data_[index];
}

void CSV::set_row(size_t index, const std::vector<std::string>& newRow) {
  if (index >= data_.size()) {
    throw std::out_of_range("Row index out of range");
  }
  data_[index] = newRow;
}

void CSV::add_row(const std::vector<std::string>& newRow) { data_.emplace_back(newRow); }

size_t CSV::row_count() const { return data_.size(); }

std::vector<std::string> CSV::split(const std::string& str, char delimiter) const {
  std::vector<std::string> tokens;
  std::stringstream ss(str);
  std::string token;
  while (std::getline(ss, token, delimiter)) {
    tokens.emplace_back(token);
  }
  return tokens;
}

std::string CSV::join(const std::vector<std::string>& elements, char delimiter) const {
  std::ostringstream ss;
  for (size_t i = 0; i < elements.size(); ++i) {
    ss << elements[i];
    if (i < elements.size() - 1) {
      ss << delimiter;
    }
  }
  return ss.str();
}

}  // namespace infra