#include "klineset.h"

namespace btra::gui {

int KLineSet::count() const { return static_cast<int>(size()); }
void KLineSet::clear() {
    std::deque<KLine>::clear();
    m_max_price = 0;
    m_min_price = 0;
    m_max_volume = 0;
}

/**
 * @brief 设置时间周期
 * @param timeframe 时间周期字符串
 */
bool KLineSet::set_timeframe(const QString& timeframe) {
    if (m_timeframe != timeframe) {
        m_timeframe = timeframe;
        return true;
    }
    return false;
}

/**
 * @brief 获取时间周期
 * @return 时间周期字符串
 */
const QString& KLineSet::get_timeframe() const { return m_timeframe; }

double KLineSet::get_max_price() const { return m_max_price; }
double KLineSet::get_min_price() const { return m_min_price; }
qint64 KLineSet::get_max_volume() const { return m_max_volume; }

} // namespace btra::gui