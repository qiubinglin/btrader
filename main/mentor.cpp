#include "mentor.h"

#include <fstream>

#include "cp_engine.h"
#include "md_engine.h"
#include "option_parser.h"
#include "td_engine.h"

namespace btra {

Mentor::Mentor(int argc, char **argv) {
  OptionParser parser;
  parser.option(0, "role", 1, [&](const char *s) { role_ = s; });
  parser.option(0, "cfg", 1, [&](const char *s) { cfg_file_ = s; });
  parser.parse(argv);
  init();
}

Mentor::~Mentor() {
  if (event_engine_) {
    delete event_engine_;
  }
}

int Mentor::run() {
  if (event_engine_) {
    event_engine_->run();
    return 0;
  } else {
    throw std::runtime_error("Event engine is not created!!!");
    return -1;
  }
}

void Mentor::init() {
  if (role_ == "cp") {
    event_engine_ = new CPEngine();
  } else if (role_ == "md") {
    event_engine_ = new MDEngine();
  } else if (role_ == "td") {
    event_engine_ = new TDEngine();
  } else {
    throw std::runtime_error("Not supported role!");
  }
  std::ifstream f(cfg_file_);
  event_engine_->setcfg(Json::json::parse(f));
}

}  // namespace btra