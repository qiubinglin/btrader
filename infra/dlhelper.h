#pragma once

#include <dlfcn.h>

#include <string>
#include <vector>

namespace infra {

class DLHelper {
 public:
  DLHelper() = default;
  ~DLHelper();

  int load(const std::string &lib);
  template <typename FuncPtr>
  FuncPtr find_symbol(int id, const std::string &symbol) {
    if (id < 0 or id > handles_.size()) {
      return nullptr;
    }
    return (FuncPtr)dlsym(handles_[id], symbol.c_str());
  }

 private:
  std::vector<void *> handles_;
};

}  // namespace infra