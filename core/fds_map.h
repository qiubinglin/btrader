#pragma once

#include <map>
#include <string>
#include <vector>

namespace btra {

class FdsMap : public std::map<std::string, int> {
public:
    static const FdsMap &get_fds_map();
    static void set_fds_file(const std::string &file);
    static void fix_fds(std::vector<std::string> &fds_vec, const std::string &socket_path);

private:
    FdsMap() {}
    ~FdsMap() {}
};

} // namespace btra