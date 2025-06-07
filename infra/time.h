#pragma once

#include <ctime>
#include <memory>
#include <string>

#define DATETIME_FORMAT "%F %T"
#define TIMESTAMP_FORMAT "%F %T.%N"
#define TRADING_DAY_FORMAT "%Y%m%d"
#define HISTORY_DAY_FORMAT "%Y-%m-%d"

namespace infra {

struct time_unit {
    static constexpr int64_t MILLISECONDS_PER_SECOND = 1000;
    static constexpr int64_t MICROSECONDS_PER_MILLISECOND = 1000;
    static constexpr int64_t NANOSECONDS_PER_MICROSECOND = 1000;

    static constexpr int64_t SECONDS_PER_MINUTE = 60;
    static constexpr int64_t MINUTES_PER_HOUR = 60;
    static constexpr int64_t HOURS_PER_DAY = 24;
    static constexpr int64_t SECONDS_PER_HOUR = SECONDS_PER_MINUTE * MINUTES_PER_HOUR;

    static constexpr int64_t NANOSECONDS_PER_MILLISECOND = NANOSECONDS_PER_MICROSECOND * MICROSECONDS_PER_MILLISECOND;
    static constexpr int64_t NANOSECONDS_PER_SECOND = NANOSECONDS_PER_MILLISECOND * MILLISECONDS_PER_SECOND;
    static constexpr int64_t NANOSECONDS_PER_MINUTE = NANOSECONDS_PER_SECOND * SECONDS_PER_MINUTE;
    static constexpr int64_t NANOSECONDS_PER_HOUR = NANOSECONDS_PER_SECOND * SECONDS_PER_HOUR;
    static constexpr int64_t NANOSECONDS_PER_DAY = NANOSECONDS_PER_HOUR * HOURS_PER_DAY;

    static constexpr int64_t UTC_OFFSET = NANOSECONDS_PER_HOUR * 8;
};

struct TimeSpec {
    int64_t sec;
    int64_t nano;

    TimeSpec operator-() const;
    TimeSpec operator+(const TimeSpec &that) const;
    TimeSpec operator-(const TimeSpec &that) const;
    TimeSpec &operator+=(const TimeSpec &that);
    TimeSpec &operator-=(const TimeSpec &that);
    int64_t to_nano() const;
    int64_t to_mili() const;
};

struct time_point_info {
    TimeSpec system_clock_count;
    TimeSpec steady_clock_count;
};

enum TimeUnit : int8_t {
    NANO,
    MILI,
    SEC,
};

class time {
private:
    time_point_info base_;
    time();

public:
    TimeUnit unit = MILI; // Default time unit is milliseconds
    static time &get_instance();
    static int64_t now_time(TimeUnit unit = get_instance().unit);

    /**
     * Get timestamp in nano seconds.
     * @return current nano time in int64_t (unix-timestamp * 1e9 + nano-part)
     */
    static int64_t now_in_nano();

    static uint32_t nano_hashed(int64_t nano_time);

    /**
     * @brief Get timestamp in seconds.
     *
     * @return int64_t
     */
    static int64_t now_in_sec();

    /**
     * @brief Get timestamp in mili-seconds.
     *
     * @return int64_t
     */
    static int64_t now_in_mili();

    /**
     * Given a timestamp, returns the start point of next minute.
     * @param nanotime timestamp in nano seconds
     * @return start time point of next minute in nano seconds
     */
    [[maybe_unused]] static int64_t next_minute(int64_t nanotime);

    /**
     * Given a timestamp, returns the next end of trading time, i.e. 15:30 of today if the argument is before that,
     * otherwise 15:30 of tomorrow.
     * @param nanotime timestamp in nano seconds
     * @return the next trading session end time point in nano seconds
     */
    static int64_t next_trading_day_end(int64_t nanotime);

    /**
     * Given a timestamp, returns the start time of the corresponding calendar day.
     * @param nanotime timesamp in nano seconds
     * @return the start time point of the corresponding calendar day in nano seconds
     */
    static int64_t calendar_day_start(int64_t nanotime);

    /**
     * Start time of today (00:00:00) in nano seconds.
     * @return start time of today in nano seconds
     */
    static int64_t today_start();

    /**
     * Parse string time to nano time.
     * @param time_string string-formatted time
     * @param format ref: https://en.cppreference.com/w/cpp/io/manip/put_time + %N for nanoseconds {:09d}
     * @return nano time in int64_t
     */
    static int64_t strptime(const std::string &time_string, const std::string &format = TIMESTAMP_FORMAT);

    /**
     * Parse string time to nano time by matching one of the formats in list.
     * @param time_string string-formatted time
     * @param formats ref: https://en.cppreference.com/w/cpp/io/manip/put_time + %N for nanoseconds {:09d}
     * @return nano time in int64_t, -1 if no format matched
     */
    static int64_t strptime(const std::string &time_string, std::initializer_list<std::string> formats);

    /**
     * Format nano seconds to string.
     * @param nanotime nano time in int64_t
     * @param format ref: https://en.cppreference.com/w/cpp/io/manip/put_time + %N for nanoseconds {:09d}
     * @return string-formatted time
     */
    static std::string strftime(int64_t nanotime, const std::string &format = TIMESTAMP_FORMAT);

    /**
     * Format now to string.
     * @param format ref: https://en.cppreference.com/w/cpp/io/manip/put_time + %N for nanoseconds {:09d}
     * @return string-formatted time
     */
    static std::string strfnow(const std::string &format = TIMESTAMP_FORMAT);

    /**
     * Gets time base point of current process.
     * @return time base point which is set by the singleton method.
     */
    static time_point_info get_base();

    /**
     * Reset time base point so that multiprocess application can have synced time.
     * @param system_clock_count std::chrono::system_clock count
     * @param steady_clock_count std::chrono::steady_clock count
     */
    static void reset(TimeSpec system_clock_count, TimeSpec steady_clock_count);
};

} // namespace infra
