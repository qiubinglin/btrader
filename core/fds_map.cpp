#include "core/fds_map.h"

#include <fstream>

#include "infra/json.h"
#include "infra/log.h"

namespace btra {

static std::string s_fds_file;

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
    if (not has_init) [[unlikely]] {
        auto env_ptr = std::getenv("FDS");
        if (env_ptr) {
            std::string fds_data = env_ptr;
            INFRA_LOG_DEBUG("fds_data: {}", fds_data);
            auto fds_vec = split(fds_data, ':');
            for (size_t i = 0; i < fds_vec.size() - 1; i += 2) {
                name2fd[fds_vec[i]] = std::stoi(fds_vec[i + 1]);
            }
        } else if (!s_fds_file.empty()) {
            std::ifstream f(s_fds_file);
            if (!f.is_open()) {
                throw std::runtime_error("Can not open: " + s_fds_file);
            }

            std::string fds_data;
            std::getline(f, fds_data);
            INFRA_LOG_DEBUG("fds_data: {}", fds_data);
            auto fds_vec = split(fds_data, ':');

            std::string socket_path;
            std::getline(f, socket_path); /* line 2: read socket path. */
            INFRA_LOG_DEBUG("socket_path: {}", socket_path);

            /* fix fds */
            fix_fds(fds_vec, socket_path);

            for (size_t i = 0; i < fds_vec.size() - 1; i += 2) {
                name2fd[fds_vec[i]] = std::stoi(fds_vec[i + 1]);
            }
        } else {
            throw std::runtime_error("init fds_map failed");
        }

        has_init = true;
    }
#endif
    return name2fd;
}

void FdsMap::set_fds_file(const std::string &file) { s_fds_file = file; }

} // namespace btra