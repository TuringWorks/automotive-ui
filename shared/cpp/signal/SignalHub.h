// SignalHub.h
// Central signal distribution and validation hub
// Part of: Shared Platform Layer
// Safety: This component is part of the safety-relevant signal chain

#ifndef AUTOMOTIVE_SIGNAL_HUB_H
#define AUTOMOTIVE_SIGNAL_HUB_H

#include <QObject>
#include <QHash>
#include <QVariant>
#include <QMutex>
#include <QElapsedTimer>
#include <memory>
#include <functional>

namespace automotive {
namespace signal {

/**
 * @brief Signal validity state
 *
 * Requirement: SR-CL-002 - Invalid signal ranges shall be clamped and flagged
 */
enum class SignalValidity : uint8_t {
    Valid = 0,           ///< Signal is within range and fresh
    Stale,               ///< Signal has not been updated within freshness window
    OutOfRange,          ///< Signal value exceeds defined limits
    Invalid,             ///< Signal failed validation (plausibility, etc.)
    NotAvailable         ///< Signal source not connected
};

/**
 * @brief Signal metadata and value container
 */
struct SignalValue {
    QVariant value;                    ///< Current signal value
    SignalValidity validity{SignalValidity::NotAvailable};
    qint64 timestampMs{0};             ///< Monotonic timestamp of last update
    qint64 sourceTimestampMs{0};       ///< Source-provided timestamp (if available)
    uint32_t updateCount{0};           ///< Number of updates received

    bool isValid() const { return validity == SignalValidity::Valid; }
    bool isDisplayable() const { return validity == SignalValidity::Valid ||
                                        validity == SignalValidity::Stale; }
};

/**
 * @brief Signal definition with validation parameters
 */
struct SignalDefinition {
    QString id;                        ///< Unique signal identifier
    QString name;                      ///< Human-readable name
    QString unit;                      ///< Unit of measurement
    QVariant minValue;                 ///< Minimum valid value
    QVariant maxValue;                 ///< Maximum valid value
    QVariant defaultValue;             ///< Default/fallback value
    qint64 freshnessMs{300};           ///< Freshness timeout in ms (SR-CL-001: 300ms)
    double maxRateOfChange{0.0};       ///< Maximum allowed rate of change (0 = disabled)
    bool isSafetyCritical{false};      ///< Safety-critical flag
};

/**
 * @brief Central signal hub for vehicle signal distribution
 *
 * This class is the single source of truth for all validated vehicle signals.
 * It enforces:
 * - Signal freshness monitoring (SR-CL-001)
 * - Range validation and clamping (SR-CL-002)
 * - Rate-of-change plausibility checks
 * - Thread-safe access
 *
 * Safety: Deterministic, bounded operations. No dynamic allocations after init.
 */
class SignalHub : public QObject {
    Q_OBJECT

public:
    explicit SignalHub(QObject* parent = nullptr);
    ~SignalHub() override;

    // Prevent copying (safety: single instance)
    SignalHub(const SignalHub&) = delete;
    SignalHub& operator=(const SignalHub&) = delete;

    /**
     * @brief Register a signal definition
     * @param def Signal definition
     * @return true if registered successfully
     *
     * Must be called during initialization phase only.
     */
    bool registerSignal(const SignalDefinition& def);

    /**
     * @brief Update a signal value from source
     * @param signalId Signal identifier
     * @param value New value
     * @param sourceTimestampMs Optional source timestamp
     * @return true if update was accepted (validation passed)
     *
     * This is the primary entry point for signal updates.
     */
    bool updateSignal(const QString& signalId,
                      const QVariant& value,
                      qint64 sourceTimestampMs = 0);

    /**
     * @brief Get current signal value with validity
     * @param signalId Signal identifier
     * @return Signal value with metadata
     */
    SignalValue getSignal(const QString& signalId) const;

    /**
     * @brief Check and update freshness for all signals
     *
     * Must be called periodically from the scheduler tick.
     * Requirement: SR-CL-001 - stale indicator within 300ms
     */
    void checkFreshness();

    /**
     * @brief Get list of all registered signal IDs
     */
    QStringList registeredSignals() const;

    /**
     * @brief Check if hub is in degraded mode
     *
     * Degraded mode activates when critical signals are invalid/stale.
     * Requirement: SR-CL-004
     */
    bool isDegradedMode() const;

    /**
     * @brief Get count of invalid/stale signals
     */
    int invalidSignalCount() const;

signals:
    /**
     * @brief Emitted when a signal value changes
     * @param signalId Signal identifier
     * @param value New signal value with metadata
     */
    void signalUpdated(const QString& signalId, const SignalValue& value);

    /**
     * @brief Emitted when signal validity changes
     * @param signalId Signal identifier
     * @param oldValidity Previous validity state
     * @param newValidity New validity state
     */
    void signalValidityChanged(const QString& signalId,
                               SignalValidity oldValidity,
                               SignalValidity newValidity);

    /**
     * @brief Emitted when degraded mode state changes
     * @param degraded true if entering degraded mode
     */
    void degradedModeChanged(bool degraded);

private:
    struct SignalState {
        SignalDefinition definition;
        SignalValue current;
        QVariant previousValue;
        qint64 previousTimestampMs{0};
    };

    bool validateRange(const SignalDefinition& def, const QVariant& value) const;
    bool validateRateOfChange(const SignalState& state,
                              const QVariant& newValue,
                              qint64 currentTimeMs) const;
    QVariant clampValue(const SignalDefinition& def, const QVariant& value) const;
    qint64 currentMonotonicTimeMs() const;

    mutable QMutex m_mutex;
    QHash<QString, SignalState> m_signals;
    QElapsedTimer m_monotonicTimer;
    bool m_degradedMode{false};
    int m_invalidCount{0};

    // Pre-allocated list for freshness checking (no dynamic alloc in steady-state)
    QStringList m_signalIdCache;
    bool m_initialized{false};
};

} // namespace signal
} // namespace automotive

#endif // AUTOMOTIVE_SIGNAL_HUB_H
