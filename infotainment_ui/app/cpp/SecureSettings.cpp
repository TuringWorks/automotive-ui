// SecureSettings.cpp
// Secure settings implementation

#include "SecureSettings.h"
#include "logging/Logger.h"
#include <QCryptographicHash>
#include <QStandardPaths>

namespace automotive {
namespace infotainment {

SecureSettings::SecureSettings(QObject* parent)
    : QObject(parent)
{
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    m_settings = std::make_unique<QSettings>(
        configPath + QStringLiteral("/settings.ini"),
        QSettings::IniFormat);
}

SecureSettings::~SecureSettings()
{
    sync();
}

QVariant SecureSettings::value(const QString& key, const QVariant& defaultValue) const
{
    if (!m_settings->contains(key)) {
        return defaultValue;
    }

    QVariant val = m_settings->value(key);

    // Decrypt if sensitive
    if (m_sensitiveKeys.contains(key)) {
        return decrypt(val);
    }

    return val;
}

void SecureSettings::setValue(const QString& key, const QVariant& value, bool sensitive)
{
    QVariant storedValue = value;

    if (sensitive) {
        m_sensitiveKeys.insert(key);
        storedValue = encrypt(value);
    }

    m_settings->setValue(key, storedValue);

    // Log without the actual value for sensitive data (CR-INF-003)
    if (sensitive) {
        AUTO_LOG_DEBUG("settings",
            QString::fromLatin1("Setting updated: %1 = [SENSITIVE]").arg(key));
    } else {
        AUTO_LOG_DEBUG("settings",
            QString::fromLatin1("Setting updated: %1").arg(key));
    }

    emit settingChanged(key);
}

void SecureSettings::remove(const QString& key)
{
    m_settings->remove(key);
    m_sensitiveKeys.remove(key);
    emit settingChanged(key);
}

bool SecureSettings::contains(const QString& key) const
{
    return m_settings->contains(key);
}

QStringList SecureSettings::keys() const
{
    return m_settings->allKeys();
}

void SecureSettings::sync()
{
    m_settings->sync();
}

QString SecureSettings::stringValue(const QString& key, const QString& defaultValue) const
{
    return value(key, defaultValue).toString();
}

int SecureSettings::intValue(const QString& key, int defaultValue) const
{
    return value(key, defaultValue).toInt();
}

bool SecureSettings::boolValue(const QString& key, bool defaultValue) const
{
    return value(key, defaultValue).toBool();
}

double SecureSettings::doubleValue(const QString& key, double defaultValue) const
{
    return value(key, defaultValue).toDouble();
}

QVariant SecureSettings::decrypt(const QVariant& encrypted) const
{
    // TODO: Implement proper encryption using platform keychain
    // For now, this is a placeholder that uses base64 encoding
    QByteArray data = QByteArray::fromBase64(encrypted.toByteArray());
    return QString::fromUtf8(data);
}

QVariant SecureSettings::encrypt(const QVariant& plaintext) const
{
    // TODO: Implement proper encryption using platform keychain
    // For now, this is a placeholder that uses base64 encoding
    QByteArray data = plaintext.toString().toUtf8().toBase64();
    return data;
}

} // namespace infotainment
} // namespace automotive
