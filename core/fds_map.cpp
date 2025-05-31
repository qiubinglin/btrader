#include "core/fds_map.h"

#include "infra/json.h"
#include <fstream>

namespace btra {

const FdsMap &FdsMap::get_fds_map() {
    static FdsMap name2fd;
    static bool has_init = false;

#ifndef HP
    if (not has_init) {
        auto env_ptr = std::getenv("FDS");
        if (env_ptr) {
            std::string fds_file = env_ptr;
            std::ifstream f(fds_file);
            Json::json fds_json = Json::json::parse(f);
            // printf("FDS: %s\n", fds_file.c_str());
            for (auto &[key, value] : fds_json.items()) {
                if (value.is_number()) {
                    name2fd[key] = value.get<int>();
                    // printf("name2fd_ %s %d\n", key.c_str(), name2fd[key]);
                }
            }
        }

        has_init = true;
    }
#endif
    return name2fd;
}

} // namespace btra