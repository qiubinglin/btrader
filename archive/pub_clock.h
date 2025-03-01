#pragma once

namespace btra {

class PubClock {
 public:
  enum Mode {
    REAL,
    RECORD,
  };
  int64_t now() const;
  void set_record_now(int64_t val);

 private:
  Mode mode_ = Mode::RECORD;
  int64_t record_now_;
};

}  // namespace btra
