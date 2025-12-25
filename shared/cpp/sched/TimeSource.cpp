// TimeSource.cpp
// Monotonic time source implementation

#include "sched/TimeSource.h"

namespace automotive {
namespace sched {

TimeSource& TimeSource::instance()
{
    static TimeSource instance;
    return instance;
}

void TimeSource::start()
{
    if (!m_timer.isValid()) {
        m_timer.start();
    }
}

qint64 TimeSource::elapsedMs() const
{
    return m_timer.isValid() ? m_timer.elapsed() : 0;
}

qint64 TimeSource::elapsedUs() const
{
    return m_timer.isValid() ? m_timer.nsecsElapsed() / 1000 : 0;
}

qint64 TimeSource::elapsedNs() const
{
    return m_timer.isValid() ? m_timer.nsecsElapsed() : 0;
}

uint64_t TimeSource::timestamp() const
{
    return static_cast<uint64_t>(elapsedMs());
}

} // namespace sched
} // namespace automotive
