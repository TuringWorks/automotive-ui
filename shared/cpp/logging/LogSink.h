// LogSink.h
// Log sink implementations
// Part of: Shared Platform Layer

#ifndef AUTOMOTIVE_LOG_SINK_H
#define AUTOMOTIVE_LOG_SINK_H

#include "logging/Logger.h"
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <memory>

namespace automotive {
namespace logging {

/**
 * @brief Console log sink
 */
class ConsoleSink : public LogSink {
public:
    ConsoleSink();
    void write(const LogEntry& entry) override;
    void flush() override;

    void setColorEnabled(bool enabled) { m_colorEnabled = enabled; }

private:
    QString formatEntry(const LogEntry& entry) const;
    QString levelString(LogLevel level) const;
    QString levelColor(LogLevel level) const;

    bool m_colorEnabled{true};
    QMutex m_mutex;
};

/**
 * @brief File log sink
 */
class FileSink : public LogSink {
public:
    explicit FileSink(const QString& filePath);
    ~FileSink() override;

    void write(const LogEntry& entry) override;
    void flush() override;

    bool isOpen() const { return m_file.isOpen(); }

    void setMaxFileSize(qint64 bytes) { m_maxFileSize = bytes; }
    void setMaxFiles(int count) { m_maxFiles = count; }

private:
    void rotateIfNeeded();
    QString formatEntry(const LogEntry& entry) const;

    QFile m_file;
    QTextStream m_stream;
    QString m_basePath;
    qint64 m_maxFileSize{10 * 1024 * 1024};  // 10MB default
    int m_maxFiles{5};
    QMutex m_mutex;
};

/**
 * @brief In-memory ring buffer sink for debugging
 */
class RingBufferSink : public LogSink {
public:
    explicit RingBufferSink(int maxEntries = 1000);

    void write(const LogEntry& entry) override;
    void flush() override;

    QVector<LogEntry> entries() const;
    void clear();

private:
    QVector<LogEntry> m_entries;
    int m_maxEntries;
    int m_writeIndex{0};
    bool m_wrapped{false};
    mutable QMutex m_mutex;
};

} // namespace logging
} // namespace automotive

#endif // AUTOMOTIVE_LOG_SINK_H
