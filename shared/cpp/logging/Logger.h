// Logger.h
// Automotive-grade logging system
// Part of: Shared Platform Layer
// Security: CR-INF-003, CR-INF-004 - PII redaction and log security

#ifndef AUTOMOTIVE_LOGGER_H
#define AUTOMOTIVE_LOGGER_H

#include <QObject>
#include <QString>
#include <QMutex>
#include <QVariantMap>
#include <QVector>
#include <memory>
#include <functional>

namespace automotive {
namespace logging {

/**
 * @brief Log severity levels
 */
enum class LogLevel : uint8_t {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Critical = 5,
    Audit = 10  // Security audit events (CR-INF-002)
};

/**
 * @brief Log entry structure
 */
struct LogEntry {
    LogLevel level;
    QString category;
    QString message;
    QString file;
    int line;
    QString function;
    qint64 timestamp;
    QString threadId;
    QVariantMap context;
};

/**
 * @brief Log sink interface
 */
class LogSink {
public:
    virtual ~LogSink() = default;
    virtual void write(const LogEntry& entry) = 0;
    virtual void flush() = 0;
};

/**
 * @brief Automotive logger with security features
 *
 * Features:
 * - Thread-safe logging
 * - Multiple sinks (console, file, network)
 * - PII redaction (CR-INF-003)
 * - Audit logging (CR-INF-002)
 * - Structured context
 */
class Logger : public QObject {
    Q_OBJECT

public:
    static Logger& instance();

    /**
     * @brief Set minimum log level
     */
    void setMinLevel(LogLevel level);
    LogLevel minLevel() const { return m_minLevel; }

    /**
     * @brief Enable/disable PII redaction
     */
    void setPiiRedactionEnabled(bool enabled);
    bool piiRedactionEnabled() const { return m_piiRedactionEnabled; }

    /**
     * @brief Add a log sink
     */
    void addSink(std::shared_ptr<LogSink> sink);

    /**
     * @brief Clear all sinks
     */
    void clearSinks();

    /**
     * @brief Log a message
     */
    void log(LogLevel level,
             const QString& category,
             const QString& message,
             const QString& file = QString(),
             int line = 0,
             const QString& function = QString(),
             const QVariantMap& context = QVariantMap());

    /**
     * @brief Log an audit event (security)
     */
    void audit(const QString& action,
               const QString& subject,
               const QString& outcome,
               const QVariantMap& details = QVariantMap());

    /**
     * @brief Flush all sinks
     */
    void flush();

signals:
    void entryLogged(const LogEntry& entry);

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    QString redactPii(const QString& message) const;
    QString formatThreadId() const;

    mutable QMutex m_mutex;
    LogLevel m_minLevel{LogLevel::Info};
    bool m_piiRedactionEnabled{true};
    QVector<std::shared_ptr<LogSink>> m_sinks;
};

// Convenience macros
#define AUTO_LOG_TRACE(cat, msg) \
    automotive::logging::Logger::instance().log( \
        automotive::logging::LogLevel::Trace, cat, msg, \
        QString::fromLatin1(__FILE__), __LINE__, QString::fromLatin1(__FUNCTION__))

#define AUTO_LOG_DEBUG(cat, msg) \
    automotive::logging::Logger::instance().log( \
        automotive::logging::LogLevel::Debug, cat, msg, \
        QString::fromLatin1(__FILE__), __LINE__, QString::fromLatin1(__FUNCTION__))

#define AUTO_LOG_INFO(cat, msg) \
    automotive::logging::Logger::instance().log( \
        automotive::logging::LogLevel::Info, cat, msg, \
        QString::fromLatin1(__FILE__), __LINE__, QString::fromLatin1(__FUNCTION__))

#define AUTO_LOG_WARNING(cat, msg) \
    automotive::logging::Logger::instance().log( \
        automotive::logging::LogLevel::Warning, cat, msg, \
        QString::fromLatin1(__FILE__), __LINE__, QString::fromLatin1(__FUNCTION__))

#define AUTO_LOG_ERROR(cat, msg) \
    automotive::logging::Logger::instance().log( \
        automotive::logging::LogLevel::Error, cat, msg, \
        QString::fromLatin1(__FILE__), __LINE__, QString::fromLatin1(__FUNCTION__))

#define AUTO_LOG_CRITICAL(cat, msg) \
    automotive::logging::Logger::instance().log( \
        automotive::logging::LogLevel::Critical, cat, msg, \
        QString::fromLatin1(__FILE__), __LINE__, QString::fromLatin1(__FUNCTION__))

} // namespace logging
} // namespace automotive

#endif // AUTOMOTIVE_LOGGER_H
