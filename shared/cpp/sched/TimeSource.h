// TimeSource.h
// Monotonic time source for safety-critical timing
// Part of: Shared Platform Layer

#ifndef AUTOMOTIVE_TIME_SOURCE_H
#define AUTOMOTIVE_TIME_SOURCE_H

#include <QObject>
#include <QElapsedTimer>
#include <cstdint>

namespace automotive {
namespace sched {

/**
 * @brief Monotonic time source
 *
 * Provides a stable, monotonic time reference for signal freshness
 * and timing calculations. Does not depend on wall clock time.
 *
 * Safety: Monotonic guarantee prevents time-related issues.
 */
class TimeSource {
public:
    /**
     * @brief Get singleton instance
     */
    static TimeSource& instance();

    /**
     * @brief Start the time source (call once at startup)
     */
    void start();

    /**
     * @brief Get elapsed time in milliseconds since start
     */
    qint64 elapsedMs() const;

    /**
     * @brief Get elapsed time in microseconds since start
     */
    qint64 elapsedUs() const;

    /**
     * @brief Get elapsed time in nanoseconds since start
     */
    qint64 elapsedNs() const;

    /**
     * @brief Check if time source is running
     */
    bool isValid() const { return m_timer.isValid(); }

    /**
     * @brief Get current timestamp suitable for signal timestamping
     */
    uint64_t timestamp() const;

private:
    TimeSource() = default;
    TimeSource(const TimeSource&) = delete;
    TimeSource& operator=(const TimeSource&) = delete;

    QElapsedTimer m_timer;
};

/**
 * @brief Convenience function to get current timestamp
 */
inline uint64_t currentTimestamp()
{
    return TimeSource::instance().timestamp();
}

/**
 * @brief Convenience function to get elapsed milliseconds
 */
inline qint64 elapsedMs()
{
    return TimeSource::instance().elapsedMs();
}

} // namespace sched
} // namespace automotive

#endif // AUTOMOTIVE_TIME_SOURCE_H
