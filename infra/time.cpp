// SPDX-License-Identifier: Apache-2.0

#include "time.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <regex>

#include "format.h"
#include "hash.h"

using namespace std::chrono;

namespace infra {

TimeSpec TimeSpec::operator-() const {
  TimeSpec res;
  res.sec = -sec;
  res.nano = -nano;
  return res;
}

TimeSpec TimeSpec::operator+(const TimeSpec &that) const {
  TimeSpec res;
  res.nano = nano + that.nano;
  res.sec = sec + that.sec + res.nano / time_unit::NANOSECONDS_PER_SECOND;
  res.nano %= time_unit::NANOSECONDS_PER_SECOND;
  return res;
}

TimeSpec TimeSpec::operator-(const TimeSpec &that) const { return *this + (-that); }

TimeSpec &TimeSpec::operator+=(const TimeSpec &that) {
  *this = *this + that;
  return *this;
}

TimeSpec &TimeSpec::operator-=(const TimeSpec &that) {
  *this = *this - that;
  return *this;
}

int64_t TimeSpec::to_nano() const { return sec * time_unit::NANOSECONDS_PER_SECOND + nano; }

#ifdef __linux__

// Make sure to use clock_gettime instead of syscall to have better performance
// https://stackoverflow.com/questions/70444744/c-linux-fastest-way-to-measure-time-faster-than-stdchrono-benchmark-incl
// https://github.com/gcc-mirror/gcc/blob/releases/gcc-11.3.0/libstdc%2B%2B-v3/src/c%2B%2B11/chrono.cc

inline TimeSpec clock_get_time(clockid_t clk_id) {
  timespec ts;
  clock_gettime(clk_id, &ts);
  return TimeSpec{ts.tv_sec, ts.tv_nsec};
}

inline int64_t get_clock_count_nano(clockid_t clk_id) {
  timespec ts;
  clock_gettime(clk_id, &ts);
  return ts.tv_sec * time_unit::NANOSECONDS_PER_SECOND + ts.tv_nsec;
}

inline int64_t system_clock_count_nano() { return get_clock_count_nano(CLOCK_REALTIME); }

inline int64_t steady_clock_count_nano() { return get_clock_count_nano(CLOCK_MONOTONIC); }

inline int64_t system_clock_count_sec() { return clock_get_time(CLOCK_REALTIME).sec; }

inline int64_t steady_clock_count_sec() { return clock_get_time(CLOCK_MONOTONIC).sec; }

TimeSpec system_clock_count() { return clock_get_time(CLOCK_REALTIME); }

TimeSpec steady_clock_count() { return clock_get_time(CLOCK_MONOTONIC); }

#else

#define NOW_SINCE_EPOCH(clock) clock::now().time_since_epoch()

inline int64_t system_clock_count() { return duration_cast<nanoseconds>(NOW_SINCE_EPOCH(system_clock)).count(); }

inline int64_t steady_clock_count() { return NOW_SINCE_EPOCH(steady_clock).count(); }

#endif

time::time() : base_() {
  base_.system_clock_count = system_clock_count();
  base_.steady_clock_count = steady_clock_count();
}

const time &time::get_instance() {
  static time instance = {};
  return instance;
}

int64_t time::now_time(TimeUnit unit) {
  switch (unit) {
    case NANO:
      return now_in_nano();
      break;
    case SEC:
    default:
      return now_in_sec();
      break;
  }
}

int64_t time::now_in_nano() {
  auto duration = steady_clock_count() - get_instance().base_.steady_clock_count;
  return (get_instance().base_.system_clock_count + duration).to_nano();
}

uint32_t time::nano_hashed(int64_t nano_time) { return hash_32((const unsigned char *)&nano_time, sizeof(nano_time)); }

int64_t time::now_in_sec() {
  auto duration = steady_clock_count() - get_instance().base_.steady_clock_count;
  return (get_instance().base_.system_clock_count + duration).sec;
}

[[maybe_unused]] int64_t time::next_minute(int64_t nanotime) {
  return nanotime - nanotime % time_unit::NANOSECONDS_PER_MINUTE + time_unit::NANOSECONDS_PER_MINUTE;
}

int64_t time::next_trading_day_end(int64_t nanotime) {
  int64_t end_offset = 15 * time_unit::NANOSECONDS_PER_HOUR + 30 * time_unit::NANOSECONDS_PER_MINUTE;
  int64_t trading_day = nanotime - nanotime % time_unit::NANOSECONDS_PER_DAY - time_unit::UTC_OFFSET + end_offset;
  if (trading_day < now_in_nano()) {
    trading_day += time_unit::NANOSECONDS_PER_DAY;
  }
  return trading_day;
}

int64_t time::calendar_day_start(int64_t nanotime) {
  return nanotime - (nanotime % time_unit::NANOSECONDS_PER_DAY) - time_unit::UTC_OFFSET;
}

int64_t time::today_start() { return calendar_day_start(time::now_in_nano()); }

int64_t time::strptime(const std::string &time_string, const std::string &format) {
  int64_t nano = 0;
  std::string normal_timestr = time_string;
  std::string normal_format = format;

  std::regex nano_format_regex("%N");
  if (std::regex_search(format, nano_format_regex)) {
    normal_format = std::regex_replace(format, nano_format_regex, "");

    std::regex nano_regex("(\\d{9})");

    auto nano_begin = std::sregex_iterator(normal_timestr.begin(), normal_timestr.end(), nano_regex);
    auto nano_end = std::sregex_iterator();
    for (std::sregex_iterator i = nano_begin; i != nano_end; ++i) {
      std::smatch match = *i;
      nano = stol(match.str());
      normal_timestr = std::regex_replace(time_string, nano_regex, "");
    }
  }

  std::tm result = {};
  std::istringstream iss(normal_timestr);
  iss >> std::get_time(&result, normal_format.c_str());
  std::time_t parsed_time = std::mktime(&result);
  auto tp_system = system_clock::from_time_t(parsed_time);
  return duration_cast<nanoseconds>(tp_system.time_since_epoch()).count() + nano;
}

int64_t time::strptime(const std::string &time_string, std::initializer_list<std::string> formats) {
  for (const auto &format : formats) {
    auto t = strptime(time_string, format);
    if (strftime(t, format) == time_string) {
      return t;
    }
  }
  return -1;
}

std::string time::strftime(int64_t nanotime, const std::string &format) {
  if (nanotime == INT64_MAX) {
    return "end of world";
  }
  time_point<steady_clock> tp_steady((nanoseconds(nanotime)));
  auto tp_epoch_steady = time_point<steady_clock>{};
  auto tp_diff = tp_steady - tp_epoch_steady;

  auto tp_epoch_system = time_point<system_clock>{};
  std::time_t time_since_epoch =
      system_clock::to_time_t(tp_epoch_system + duration_cast<system_clock::duration>(tp_diff));

  std::string normal_format = format;
  std::regex nano_format_regex("%N");
  if (std::regex_search(format, nano_format_regex)) {
    int64_t nano = tp_diff.count() % time_unit::NANOSECONDS_PER_SECOND;
    normal_format = std::regex_replace(format, nano_format_regex, fmt::format("{:09d}", nano));
  }

  std::ostringstream oss;
  oss << std::put_time(std::localtime(&time_since_epoch), normal_format.c_str());
  if (nanotime > 0) {
    return oss.str();
  } else if (nanotime == 0) {
    return std::regex_replace(oss.str(), std::regex("\\d"), "0");
  } else {
    return std::regex_replace(oss.str(), std::regex("\\d"), "#");
  }
}

std::string time::strfnow(const std::string &format) { return strftime(now_in_nano(), format); }

time_point_info time::get_base() { return get_instance().base_; }

void time::reset(TimeSpec system_clock_count, TimeSpec steady_clock_count) {
  time_point_info &base = const_cast<time &>(get_instance()).base_;
  base.system_clock_count = system_clock_count;
  base.steady_clock_count = steady_clock_count;
}

}  // namespace infra
