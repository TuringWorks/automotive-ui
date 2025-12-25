// TelltaleManager.h
// Telltale state management
// Safety: SR-CL-003 - Critical telltales shall remain visible

#ifndef AUTOMOTIVE_TELLTALE_MANAGER_H
#define AUTOMOTIVE_TELLTALE_MANAGER_H

#include <QObject>
#include <QHash>
#include <QString>
#include <QVariantList>
#include "signal/SignalHub.h"

namespace automotive {
namespace driver {

/**
 * @brief Telltale state
 */
struct TelltaleState {
    QString id;
    QString name;
    QString iconPath;
    int priority{2};     // 0=critical, 1=warning, 2=info, 3=status
    bool active{false};
    bool valid{true};
    bool blinking{false};
};

/**
 * @brief Telltale manager for cluster
 *
 * Manages all telltale states from signal hub.
 * Safety: SR-CL-003 - Critical telltales always visible.
 */
class TelltaleManager : public QObject {
    Q_OBJECT

    Q_PROPERTY(QVariantList activeTelltales READ activeTelltalesToVariant NOTIFY activeTelltalesChanged)
    Q_PROPERTY(QVariantList criticalTelltales READ criticalTelltalesToVariant NOTIFY criticalTelltalesChanged)
    Q_PROPERTY(int activeCount READ activeCount NOTIFY activeCountChanged)
    Q_PROPERTY(bool hasCritical READ hasCritical NOTIFY hasCriticalChanged)

public:
    explicit TelltaleManager(signal::SignalHub* signalHub, QObject* parent = nullptr);
    ~TelltaleManager() override;

    /**
     * @brief Register a telltale
     */
    void registerTelltale(const QString& signalId,
                          const QString& name,
                          const QString& iconPath,
                          int priority);

    /**
     * @brief Get all active telltales
     */
    QVector<TelltaleState> activeTelltales() const;
    QVariantList activeTelltalesToVariant() const;

    /**
     * @brief Get critical telltales only
     */
    QVector<TelltaleState> criticalTelltales() const;
    QVariantList criticalTelltalesToVariant() const;

    /**
     * @brief Get telltale state by ID
     */
    Q_INVOKABLE QVariantMap getTelltale(const QString& id) const;

    int activeCount() const;
    bool hasCritical() const;

    /**
     * @brief Initialize default telltales
     */
    void initializeDefaults();

signals:
    void activeTelltalesChanged();
    void criticalTelltalesChanged();
    void activeCountChanged(int count);
    void hasCriticalChanged(bool hasCritical);
    void telltaleActivated(const QString& id, int priority);
    void telltaleDeactivated(const QString& id);

private slots:
    void onSignalUpdated(const QString& signalId, const signal::SignalValue& value);

private:
    TelltaleState toState(const QString& signalId) const;
    QVariantMap stateToVariant(const TelltaleState& state) const;

    signal::SignalHub* m_signalHub{nullptr};

    struct TelltaleConfig {
        QString name;
        QString iconPath;
        int priority;
        bool active;
        bool valid;
    };

    QHash<QString, TelltaleConfig> m_telltales;
    int m_activeCount{0};
    bool m_hasCritical{false};
};

} // namespace driver
} // namespace automotive

#endif // AUTOMOTIVE_TELLTALE_MANAGER_H
