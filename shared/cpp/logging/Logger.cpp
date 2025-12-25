// Logger.cpp
// Automotive logger implementation

#include "logging/Logger.h"
#include "logging/PiiRedactor.h"
#include <QDateTime>
#include <QThread>
#include <QDebug>

namespace automotive {
namespace logging {

Logger& Logger::instance()
{
    static Logger instance;
    return instance;
}

Logger::Logger()
{
}

Logger::~Logger()
{
    flush();
}

void Logger::setMinLevel(LogLevel level)
{
    QMutexLocker locker(&m_mutex);
    m_minLevel = level;
}

void Logger::setPiiRedactionEnabled(bool enabled)
{
    QMutexLocker locker(&m_mutex);
    m_piiRedactionEnabled = enabled;
}

void Logger::addSink(std::shared_ptr<LogSink> sink)
{
    QMutexLocker locker(&m_mutex);
    m_sinks.append(sink);
}

void Logger::clearSinks()
{
    QMutexLocker locker(&m_mutex);
    for (auto& sink : m_sinks) {
        sink->flush();
    }
    m_sinks.clear();
}

void Logger::log(LogLevel level,
                 const QString& category,
                 const QString& message,
                 const QString& file,
                 int line,
                 const QString& function,
                 const QVariantMap& context)
{
    QMutexLocker locker(&m_mutex);

    if (level < m_minLevel && level != LogLevel::Audit) {
        return;
    }

    LogEntry entry;
    entry.level = level;
    entry.category = category;
    entry.message = m_piiRedactionEnabled ? redactPii(message) : message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = QDateTime::currentMSecsSinceEpoch();
    entry.threadId = formatThreadId();
    entry.context = context;

    for (auto& sink : m_sinks) {
        sink->write(entry);
    }

    locker.unlock();
    emit entryLogged(entry);
}

void Logger::audit(const QString& action,
                   const QString& subject,
                   const QString& outcome,
                   const QVariantMap& details)
{
    QVariantMap context = details;
    context.insert(QStringLiteral("action"), action);
    context.insert(QStringLiteral("subject"), subject);
    context.insert(QStringLiteral("outcome"), outcome);

    QString message = QString::fromLatin1("AUDIT: %1 on %2 - %3")
        .arg(action, subject, outcome);

    log(LogLevel::Audit, QStringLiteral("security.audit"), message,
        QString(), 0, QString(), context);
}

void Logger::flush()
{
    QMutexLocker locker(&m_mutex);
    for (auto& sink : m_sinks) {
        sink->flush();
    }
}

QString Logger::redactPii(const QString& message) const
{
    return PiiRedactor::redact(message);
}

QString Logger::formatThreadId() const
{
    return QString::number(
        reinterpret_cast<quintptr>(QThread::currentThreadId()), 16);
}

} // namespace logging
} // namespace automotive
