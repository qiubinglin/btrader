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
    static constexpr int64_t SECONDS_PER_DAY = SECONDS_PER_HOUR * HOURS_PER_DAY;

    static constexpr int64_t MILLISECONDS_PER_MINUTE = MILLISECONDS_PER_SECOND * SECONDS_PER_MINUTE;
    static constexpr int64_t MILLISECONDS_PER_HOUR = MILLISECONDS_PER_MINUTE * MINUTES_PER_HOUR;
    static constexpr int64_t MILLISECONDS_PER_DAY = MILLISECONDS_PER_HOUR * HOURS_PER_DAY;

    static constexpr int64_t NANOSECONDS_PER_MILLISECOND = NANOSECONDS_PER_MICROSECOND * MICROSECONDS_PER_MILLISECOND;
    static constexpr int64_t NANOSECONDS_PER_SECOND = NANOSECONDS_PER_MILLISECOND * MILLISECONDS_PER_SECOND;
    static constexpr int64_t NANOSECONDS_PER_MINUTE = NANOSECONDS_PER_SECOND * SECONDS_PER_MINUTE;
    static constexpr int64_t NANOSECONDS_PER_HOUR = NANOSECONDS_PER_SECOND * SECONDS_PER_HOUR;
    static constexpr int64_t NANOSECONDS_PER_DAY = NANOSECONDS_PER_HOUR * HOURS_PER_DAY;

    /* Beijing time zone */
    static constexpr int64_t NANO_UTC_OFFSET = NANOSECONDS_PER_HOUR * 8;
    static constexpr int64_t MILI_UTC_OFFSET = MILLISECONDS_PER_HOUR * 8;
    static constexpr int64_t SEC_UTC_OFFSET = SECONDS_PER_HOUR * 8;
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
    MILLI,
    SEC,
};

class time {
private:
    time_point_info base_;
    time();

public:
    TimeUnit unit = MILLI; // Default time unit is milliseconds
    static time &get_instance();
    static int64_t now_time(TimeUnit unit = get_instance().unit);

    /**
     * @brief Get timestamp in nano seconds.
     * @return current nano time in int64_t (unix-timestamp * 1e9 + nano-part)
     */
    static int64_t now_in_nano();

    static uint32_t time_hashed(int64_t timestamp);

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
     * @brief Given a timestamp, returns the start point of next minute.
     * @param timestamp
     * @return start time point of next minute
     */
    [[maybe_unused]] static int64_t next_minute(int64_t timestamp);

    /**
     * @brief Given a timestamp, returns the next end of trading time, i.e. 15:30 of today if the argument is before that,
     * otherwise 15:30 of tomorrow.
     * @param timestamp timestamp
     * @return the next trading session end time point
     */
    static int64_t next_trading_day_end(int64_t timestamp);

    /**
     * @brief Given a timestamp, returns the start time of the corresponding calendar day (00:00:00).
     * @param timestamp timesamp
     * @return the start time point of the corresponding calendar day
     */
    static int64_t calendar_day_start(int64_t timestamp);

    /**
     * @brief Start time of today (00:00:00).
     * @return start time of today
     */
    static int64_t today_start();

    /**
     * @brief Parse string time to timestamp.
     * @param time_string string-formatted time
     * @param format ref: https://en.cppreference.com/w/cpp/io/manip/put_time + %N for nanoseconds {:09d}
     * @return timestamp in int64_t
     */
    static int64_t strptime(const std::string &time_string, const std::string &format = TIMESTAMP_FORMAT);

    /**
     * @brief Parse string time to timestamp by matching one of the formats in list.
     * @param time_string string-formatted time
     * @param formats ref: https://en.cppreference.com/w/cpp/io/manip/put_time + %N for nanoseconds {:09d}
     * @return timestamp in int64_t, -1 if no format matched
     */
    static int64_t strptime(const std::string &time_string, std::initializer_list<std::string> formats);

    /**
     * Format timestamp to string.
     * @param timestamp timestamp in int64_t
     * @param format ref: https://en.cppreference.com/w/cpp/io/manip/put_time + %N for nanoseconds {:09d}
     * @return string-formatted time
     */
    static std::string strftime(int64_t timestamp, const std::string &format = TIMESTAMP_FORMAT);

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

    static void strptimerange(const std::string &time_string, const std::string &format, int64_t &start_time, int64_t &end_time);
};

} // namespace infra
