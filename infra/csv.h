#pragma once

#include <string>
#include <vector>

namespace infra {

class CSV {
 public:
  bool read(const std::string& filename);

  bool write(const std::string& filename) const;

  const std::vector<std::string>& get_row(size_t index) const;

  void set_row(size_t index, const std::vector<std::string>& newRow);

  void add_row(const std::vector<std::string>& newRow);

  size_t row_count() const;

 private:
  std::vector<std::string> split(const std::string& str, char delimiter) const;
  std::string join(const std::vector<std::string>& elements, char delimiter) const;

 private:
  std::vector<std::vector<std::string>> data_;
};

}  // namespace infra