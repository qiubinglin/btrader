#include "core/fds_map.h"

#include "infra/json.h"
#include <fstream>

namespace btra {

static std::vector<std::string> split(const std::string &str, char deli) {
    std::vector<std::string> res;
    std::size_t beg = 0;
    std::size_t pos;
    pos = str.find(deli, beg);
    while (pos != std::string::npos) {
        res.push_back(str.substr(beg, pos - beg));
        beg = pos + 1;
        pos = str.find(deli, beg);
    }
    if (beg < str.size()) {
        res.push_back(str.substr(beg, str.size() - beg));
    }
    return res;
}

const FdsMap &FdsMap::get_fds_map() {
    static FdsMap name2fd;
    static bool has_init = false;

#ifndef HP
    if (not has_init) {
        auto env_ptr = std::getenv("FDS");
        if (env_ptr) {
            std::string fds_data = env_ptr;
            // printf("%s\n", fds_data.c_str());
            auto fds_vec = split(fds_data, ':');
            for (size_t i = 0; i < fds_vec.size() - 1; i += 2) {
                name2fd[fds_vec[i]] = std::stoi(fds_vec[i + 1]);
            }
        }

        has_init = true;
    }
#endif
    return name2fd;
}

} // namespace btra