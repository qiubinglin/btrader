#pragma once

#include "eventengine.h"

namespace btra {

class Mentor {
public:
    Mentor(int argc, char **argv);
    ~Mentor();

    int run();

private:
    void init();
    void setup(const std::string &id);

    int _run();

    std::string role_;
    std::string cfg_file_;
    EventEngine *event_engine_ = nullptr;
};

} // namespace btra