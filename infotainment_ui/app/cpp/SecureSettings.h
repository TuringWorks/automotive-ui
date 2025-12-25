// SecureSettings.h
// Secure settings storage
// Security: CR-INF-003 - No sensitive data in logs, secure storage

#ifndef AUTOMOTIVE_SECURE_SETTINGS_H
#define AUTOMOTIVE_SECURE_SETTINGS_H

#include <QObject>
#include <QVariant>
#include <QSettings>
#include <memory>

namespace automotive {
namespace infotainment {

/**
 * @brief Secure settings storage
 *
 * Security: CR-INF-003, CR-INF-004
 * - Values are stored securely (OS keychain in production)
 * - PII is not logged
 * - Sensitive values are encrypted
 */
class SecureSettings : public QObject {
    Q_OBJECT

public:
    explicit SecureSettings(QObject* parent = nullptr);
    ~SecureSettings() override;

    /**
     * @brief Get a setting value
     * @param key Setting key
     * @param defaultValue Default if not found
     */
    Q_INVOKABLE QVariant value(const QString& key,
                               const QVariant& defaultValue = QVariant()) const;

    /**
     * @brief Set a setting value
     * @param key Setting key
     * @param value Value to store
     * @param sensitive If true, value is encrypted
     */
    Q_INVOKABLE void setValue(const QString& key,
                               const QVariant& value,
                               bool sensitive = false);

    /**
     * @brief Remove a setting
     */
    Q_INVOKABLE void remove(const QString& key);

    /**
     * @brief Check if a setting exists
     */
    Q_INVOKABLE bool contains(const QString& key) const;

    /**
     * @brief Get all keys
     */
    Q_INVOKABLE QStringList keys() const;

    /**
     * @brief Sync settings to storage
     */
    Q_INVOKABLE void sync();

    // Convenience typed accessors
    QString stringValue(const QString& key, const QString& defaultValue = QString()) const;
    int intValue(const QString& key, int defaultValue = 0) const;
    bool boolValue(const QString& key, bool defaultValue = false) const;
    double doubleValue(const QString& key, double defaultValue = 0.0) const;

signals:
    void settingChanged(const QString& key);

private:
    QVariant decrypt(const QVariant& encrypted) const;
    QVariant encrypt(const QVariant& plaintext) const;

    std::unique_ptr<QSettings> m_settings;
    QSet<QString> m_sensitiveKeys;
};

} // namespace infotainment
} // namespace automotive

#endif // AUTOMOTIVE_SECURE_SETTINGS_H
