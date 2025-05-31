#pragma once

#include <map>
#include <string>

namespace btra {

class FdsMap : public std::map<std::string, int> {
public:
    static const FdsMap &get_fds_map();

private:
    FdsMap() {}
    ~FdsMap() {}
};

}