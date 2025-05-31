#include "dlhelper.h"

#include <iostream>

namespace infra {

DLHelper::~DLHelper() {
    for (auto handle : handles_) {
        dlclose(handle);
    }
}

int DLHelper::load(const std::string &lib) {
    void *handle = dlopen(lib.c_str(), RTLD_LAZY);
    if (not handle) {
        std::cerr << "Error loading library: " << dlerror() << std::endl;
        return -1;
    }
    handles_.push_back(handle);
    return static_cast<int>(handles_.size()) - 1;
}

} // namespace infra