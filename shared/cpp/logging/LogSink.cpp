// LogSink.cpp
// Log sink implementations

#include "logging/LogSink.h"
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <iostream>

namespace automotive {
namespace logging {

// ConsoleSink implementation

ConsoleSink::ConsoleSink()
{
}

void ConsoleSink::write(const LogEntry& entry)
{
    QMutexLocker locker(&m_mutex);
    std::cout << formatEntry(entry).toStdString() << std::endl;
}

void ConsoleSink::flush()
{
    std::cout.flush();
}

QString ConsoleSink::formatEntry(const LogEntry& entry) const
{
    QString timestamp = QDateTime::fromMSecsSinceEpoch(entry.timestamp)
        .toString(Qt::ISODateWithMs);

    QString level = levelString(entry.level);

    QString result;
    if (m_colorEnabled) {
        result = QString::fromLatin1("%1[%2] [%3] [%4] %5\033[0m")
            .arg(levelColor(entry.level), timestamp, level, entry.category, entry.message);
    } else {
        result = QString::fromLatin1("[%1] [%2] [%3] %4")
            .arg(timestamp, level, entry.category, entry.message);
    }

    return result;
}

QString ConsoleSink::levelString(LogLevel level) const
{
    switch (level) {
    case LogLevel::Trace:    return QStringLiteral("TRACE");
    case LogLevel::Debug:    return QStringLiteral("DEBUG");
    case LogLevel::Info:     return QStringLiteral("INFO ");
    case LogLevel::Warning:  return QStringLiteral("WARN ");
    case LogLevel::Error:    return QStringLiteral("ERROR");
    case LogLevel::Critical: return QStringLiteral("CRIT ");
    case LogLevel::Audit:    return QStringLiteral("AUDIT");
    default:                 return QStringLiteral("?????");
    }
}

QString ConsoleSink::levelColor(LogLevel level) const
{
    switch (level) {
    case LogLevel::Trace:    return QStringLiteral("\033[90m");     // Gray
    case LogLevel::Debug:    return QStringLiteral("\033[36m");     // Cyan
    case LogLevel::Info:     return QStringLiteral("\033[32m");     // Green
    case LogLevel::Warning:  return QStringLiteral("\033[33m");     // Yellow
    case LogLevel::Error:    return QStringLiteral("\033[31m");     // Red
    case LogLevel::Critical: return QStringLiteral("\033[1;31m");   // Bold Red
    case LogLevel::Audit:    return QStringLiteral("\033[35m");     // Magenta
    default:                 return QStringLiteral("\033[0m");
    }
}

// FileSink implementation

FileSink::FileSink(const QString& filePath)
    : m_basePath(filePath)
{
    m_file.setFileName(filePath);
    if (m_file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        m_stream.setDevice(&m_file);
    }
}

FileSink::~FileSink()
{
    flush();
    m_file.close();
}

void FileSink::write(const LogEntry& entry)
{
    QMutexLocker locker(&m_mutex);

    if (!m_file.isOpen()) {
        return;
    }

    rotateIfNeeded();

    m_stream << formatEntry(entry) << Qt::endl;
}

void FileSink::flush()
{
    QMutexLocker locker(&m_mutex);
    if (m_file.isOpen()) {
        m_stream.flush();
        m_file.flush();
    }
}

void FileSink::rotateIfNeeded()
{
    if (m_file.size() < m_maxFileSize) {
        return;
    }

    m_stream.flush();
    m_file.close();

    // Rotate existing files
    QFileInfo info(m_basePath);
    QString baseName = info.completeBaseName();
    QString suffix = info.suffix();
    QString dirPath = info.absolutePath();

    for (int i = m_maxFiles - 1; i > 0; --i) {
        QString oldName = QString::fromLatin1("%1/%2.%3.%4")
            .arg(dirPath, baseName, QString::number(i), suffix);
        QString newName = QString::fromLatin1("%1/%2.%3.%4")
            .arg(dirPath, baseName, QString::number(i + 1), suffix);

        QFile::remove(newName);
        QFile::rename(oldName, newName);
    }

    QString firstRotated = QString::fromLatin1("%1/%2.1.%3")
        .arg(dirPath, baseName, suffix);
    QFile::rename(m_basePath, firstRotated);

    // Reopen fresh file
    m_file.setFileName(m_basePath);
    m_file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    m_stream.setDevice(&m_file);
}

QString FileSink::formatEntry(const LogEntry& entry) const
{
    QString timestamp = QDateTime::fromMSecsSinceEpoch(entry.timestamp)
        .toString(Qt::ISODateWithMs);

    QString level;
    switch (entry.level) {
    case LogLevel::Trace:    level = QStringLiteral("TRACE"); break;
    case LogLevel::Debug:    level = QStringLiteral("DEBUG"); break;
    case LogLevel::Info:     level = QStringLiteral("INFO"); break;
    case LogLevel::Warning:  level = QStringLiteral("WARN"); break;
    case LogLevel::Error:    level = QStringLiteral("ERROR"); break;
    case LogLevel::Critical: level = QStringLiteral("CRIT"); break;
    case LogLevel::Audit:    level = QStringLiteral("AUDIT"); break;
    default:                 level = QStringLiteral("?"); break;
    }

    return QString::fromLatin1("[%1] [%2] [%3] [%4] %5")
        .arg(timestamp, entry.threadId, level, entry.category, entry.message);
}

// RingBufferSink implementation

RingBufferSink::RingBufferSink(int maxEntries)
    : m_maxEntries(maxEntries)
{
    m_entries.resize(maxEntries);
}

void RingBufferSink::write(const LogEntry& entry)
{
    QMutexLocker locker(&m_mutex);

    m_entries[m_writeIndex] = entry;
    m_writeIndex = (m_writeIndex + 1) % m_maxEntries;

    if (m_writeIndex == 0) {
        m_wrapped = true;
    }
}

void RingBufferSink::flush()
{
    // No-op for ring buffer
}

QVector<LogEntry> RingBufferSink::entries() const
{
    QMutexLocker locker(&m_mutex);

    if (!m_wrapped) {
        return m_entries.mid(0, m_writeIndex);
    }

    // Return in chronological order
    QVector<LogEntry> result;
    result.reserve(m_maxEntries);
    result.append(m_entries.mid(m_writeIndex));
    result.append(m_entries.mid(0, m_writeIndex));
    return result;
}

void RingBufferSink::clear()
{
    QMutexLocker locker(&m_mutex);
    m_writeIndex = 0;
    m_wrapped = false;
}

} // namespace logging
} // namespace automotive
