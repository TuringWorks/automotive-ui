// DegradedModeController.h
// Degraded mode state machine
// Safety: SR-CL-004 - Degraded mode on loss of signal integrity

#ifndef AUTOMOTIVE_DEGRADED_MODE_CONTROLLER_H
#define AUTOMOTIVE_DEGRADED_MODE_CONTROLLER_H

#include <QObject>
#include "signal/SignalHub.h"

namespace automotive {
namespace driver {

/**
 * @brief Degraded mode level
 */
enum class DegradedLevel : uint8_t {
    Normal = 0,      ///< All systems nominal
    Partial = 1,     ///< Some non-critical signals missing
    Degraded = 2,    ///< Critical signals missing or stale
    Minimal = 3      ///< Minimal safe display only
};

/**
 * @brief Degraded mode controller
 *
 * Safety: SR-CL-004 - Degraded mode shall activate on loss of signal integrity.
 * Controls what UI elements are shown based on signal health.
 */
class DegradedModeController : public QObject {
    Q_OBJECT

    Q_PROPERTY(DegradedLevel level READ level NOTIFY levelChanged)
    Q_PROPERTY(bool isDegraded READ isDegraded NOTIFY isDegradedChanged)
    Q_PROPERTY(bool showSpeedGauge READ showSpeedGauge NOTIFY visibilityChanged)
    Q_PROPERTY(bool showSecondaryGauges READ showSecondaryGauges NOTIFY visibilityChanged)
    Q_PROPERTY(bool showTelltales READ showTelltales NOTIFY visibilityChanged)
    Q_PROPERTY(bool showAlerts READ showAlerts NOTIFY visibilityChanged)
    Q_PROPERTY(bool showInfoPanel READ showInfoPanel NOTIFY visibilityChanged)
    Q_PROPERTY(QString degradedMessage READ degradedMessage NOTIFY degradedMessageChanged)

public:
    explicit DegradedModeController(signal::SignalHub* signalHub, QObject* parent = nullptr);
    ~DegradedModeController() override;

    DegradedLevel level() const { return m_level; }
    bool isDegraded() const { return m_level >= DegradedLevel::Degraded; }

    // Visibility controls based on degraded level
    bool showSpeedGauge() const { return true; }  // Always shown
    bool showSecondaryGauges() const { return m_level <= DegradedLevel::Partial; }
    bool showTelltales() const { return true; }  // Always shown (SR-CL-003)
    bool showAlerts() const { return true; }  // Always shown (SR-CL-003)
    bool showInfoPanel() const { return m_level <= DegradedLevel::Normal; }

    QString degradedMessage() const { return m_degradedMessage; }

    /**
     * @brief Process tick - evaluate degraded state
     */
    void processTick(qint64 currentTimeMs);

    /**
     * @brief Force degraded level (for testing)
     */
    Q_INVOKABLE void forceLevel(int level);

    /**
     * @brief Clear forced level
     */
    Q_INVOKABLE void clearForcedLevel();

signals:
    void levelChanged(DegradedLevel level);
    void isDegradedChanged(bool degraded);
    void visibilityChanged();
    void degradedMessageChanged(const QString& message);
    void enteringDegradedMode(DegradedLevel level);
    void exitingDegradedMode();

private slots:
    void onDegradedModeChanged(bool degraded);

private:
    void updateLevel();
    void setLevel(DegradedLevel newLevel);
    QString levelToMessage(DegradedLevel level) const;

    signal::SignalHub* m_signalHub{nullptr};
    DegradedLevel m_level{DegradedLevel::Normal};
    DegradedLevel m_forcedLevel{DegradedLevel::Normal};
    bool m_levelForced{false};
    QString m_degradedMessage;
};

} // namespace driver
} // namespace automotive

Q_DECLARE_METATYPE(automotive::driver::DegradedLevel)

#endif // AUTOMOTIVE_DEGRADED_MODE_CONTROLLER_H
