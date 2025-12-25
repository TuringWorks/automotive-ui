// SignalHub.cpp
// Central signal distribution and validation hub implementation

#include "signal/SignalHub.h"
#include <QDebug>
#include <cmath>

namespace automotive {
namespace signal {

SignalHub::SignalHub(QObject* parent)
    : QObject(parent)
{
    m_monotonicTimer.start();
}

SignalHub::~SignalHub() = default;

bool SignalHub::registerSignal(const SignalDefinition& def)
{
    if (def.id.isEmpty()) {
        qWarning() << "SignalHub: Cannot register signal with empty ID";
        return false;
    }

    QMutexLocker locker(&m_mutex);

    if (m_initialized) {
        qWarning() << "SignalHub: Cannot register signals after initialization";
        return false;
    }

    if (m_signals.contains(def.id)) {
        qWarning() << "SignalHub: Signal already registered:" << def.id;
        return false;
    }

    SignalState state;
    state.definition = def;
    state.current.value = def.defaultValue;
    state.current.validity = SignalValidity::NotAvailable;
    state.current.timestampMs = 0;
    state.current.updateCount = 0;

    m_signals.insert(def.id, state);
    m_signalIdCache.append(def.id);

    return true;
}

bool SignalHub::updateSignal(const QString& signalId,
                              const QVariant& value,
                              qint64 sourceTimestampMs)
{
    QMutexLocker locker(&m_mutex);

    auto it = m_signals.find(signalId);
    if (it == m_signals.end()) {
        qWarning() << "SignalHub: Unknown signal:" << signalId;
        return false;
    }

    SignalState& state = it.value();
    const qint64 currentTimeMs = currentMonotonicTimeMs();

    // Mark as initialized on first update
    if (!m_initialized) {
        m_initialized = true;
    }

    SignalValidity oldValidity = state.current.validity;
    SignalValidity newValidity = SignalValidity::Valid;
    QVariant finalValue = value;

    // Validate range (SR-CL-002)
    if (!validateRange(state.definition, value)) {
        if (state.definition.isSafetyCritical) {
            // Critical signals: clamp and mark invalid
            finalValue = clampValue(state.definition, value);
            newValidity = SignalValidity::OutOfRange;
        } else {
            // Non-critical: clamp silently
            finalValue = clampValue(state.definition, value);
        }
    }

    // Validate rate of change (plausibility)
    if (state.definition.maxRateOfChange > 0.0 &&
        state.current.validity == SignalValidity::Valid) {
        if (!validateRateOfChange(state, value, currentTimeMs)) {
            newValidity = SignalValidity::Invalid;
            qWarning() << "SignalHub: Rate-of-change violation for" << signalId;
        }
    }

    // Store previous for rate-of-change calculation
    state.previousValue = state.current.value;
    state.previousTimestampMs = state.current.timestampMs;

    // Update current value
    state.current.value = finalValue;
    state.current.validity = newValidity;
    state.current.timestampMs = currentTimeMs;
    state.current.sourceTimestampMs = sourceTimestampMs;
    state.current.updateCount++;

    // Track invalid count for degraded mode
    if (oldValidity == SignalValidity::Valid &&
        newValidity != SignalValidity::Valid) {
        m_invalidCount++;
    } else if (oldValidity != SignalValidity::Valid &&
               newValidity == SignalValidity::Valid) {
        m_invalidCount = qMax(0, m_invalidCount - 1);
    }

    // Check degraded mode transition (SR-CL-004)
    bool shouldBeDegraded = m_invalidCount > 0;
    if (shouldBeDegraded != m_degradedMode) {
        m_degradedMode = shouldBeDegraded;
        locker.unlock();
        emit degradedModeChanged(m_degradedMode);
        locker.relock();
    }

    // Emit updates
    SignalValue valueCopy = state.current;
    locker.unlock();

    emit signalUpdated(signalId, valueCopy);

    if (oldValidity != newValidity) {
        emit signalValidityChanged(signalId, oldValidity, newValidity);
    }

    return newValidity == SignalValidity::Valid;
}

SignalValue SignalHub::getSignal(const QString& signalId) const
{
    QMutexLocker locker(&m_mutex);

    auto it = m_signals.find(signalId);
    if (it == m_signals.end()) {
        SignalValue invalid;
        invalid.validity = SignalValidity::NotAvailable;
        return invalid;
    }

    return it.value().current;
}

void SignalHub::checkFreshness()
{
    QMutexLocker locker(&m_mutex);

    const qint64 currentTimeMs = currentMonotonicTimeMs();
    QVector<QPair<QString, QPair<SignalValidity, SignalValidity>>> validityChanges;

    // Use pre-allocated cache to avoid allocations
    for (const QString& signalId : qAsConst(m_signalIdCache)) {
        SignalState& state = m_signals[signalId];

        if (state.current.validity == SignalValidity::Valid) {
            const qint64 age = currentTimeMs - state.current.timestampMs;

            // Requirement: SR-CL-001 - stale indicator within freshnessMs
            if (age > state.definition.freshnessMs) {
                SignalValidity oldValidity = state.current.validity;
                state.current.validity = SignalValidity::Stale;

                m_invalidCount++;
                validityChanges.append({signalId, {oldValidity, SignalValidity::Stale}});
            }
        }
    }

    // Check degraded mode (SR-CL-004)
    bool shouldBeDegraded = m_invalidCount > 0;
    bool degradedChanged = (shouldBeDegraded != m_degradedMode);
    m_degradedMode = shouldBeDegraded;

    locker.unlock();

    // Emit signals outside of lock
    for (const auto& change : validityChanges) {
        emit signalValidityChanged(change.first, change.second.first, change.second.second);
    }

    if (degradedChanged) {
        emit degradedModeChanged(m_degradedMode);
    }
}

QStringList SignalHub::registeredSignals() const
{
    QMutexLocker locker(&m_mutex);
    return m_signalIdCache;
}

bool SignalHub::isDegradedMode() const
{
    QMutexLocker locker(&m_mutex);
    return m_degradedMode;
}

int SignalHub::invalidSignalCount() const
{
    QMutexLocker locker(&m_mutex);
    return m_invalidCount;
}

bool SignalHub::validateRange(const SignalDefinition& def, const QVariant& value) const
{
    if (!def.minValue.isValid() && !def.maxValue.isValid()) {
        return true; // No range defined
    }

    bool ok = false;
    double numValue = value.toDouble(&ok);
    if (!ok) {
        return true; // Non-numeric, skip range check
    }

    if (def.minValue.isValid()) {
        double minVal = def.minValue.toDouble();
        if (numValue < minVal) {
            return false;
        }
    }

    if (def.maxValue.isValid()) {
        double maxVal = def.maxValue.toDouble();
        if (numValue > maxVal) {
            return false;
        }
    }

    return true;
}

bool SignalHub::validateRateOfChange(const SignalState& state,
                                      const QVariant& newValue,
                                      qint64 currentTimeMs) const
{
    if (state.previousTimestampMs == 0) {
        return true; // First update, no previous value
    }

    bool ok1 = false, ok2 = false;
    double prevVal = state.previousValue.toDouble(&ok1);
    double newVal = newValue.toDouble(&ok2);

    if (!ok1 || !ok2) {
        return true; // Non-numeric, skip check
    }

    qint64 deltaTimeMs = currentTimeMs - state.previousTimestampMs;
    if (deltaTimeMs <= 0) {
        return true; // Prevent division by zero
    }

    double deltaValue = std::abs(newVal - prevVal);
    double ratePerSecond = (deltaValue * 1000.0) / static_cast<double>(deltaTimeMs);

    return ratePerSecond <= state.definition.maxRateOfChange;
}

QVariant SignalHub::clampValue(const SignalDefinition& def, const QVariant& value) const
{
    bool ok = false;
    double numValue = value.toDouble(&ok);
    if (!ok) {
        return def.defaultValue;
    }

    if (def.minValue.isValid()) {
        double minVal = def.minValue.toDouble();
        numValue = qMax(numValue, minVal);
    }

    if (def.maxValue.isValid()) {
        double maxVal = def.maxValue.toDouble();
        numValue = qMin(numValue, maxVal);
    }

    return QVariant(numValue);
}

qint64 SignalHub::currentMonotonicTimeMs() const
{
    return m_monotonicTimer.elapsed();
}

} // namespace signal
} // namespace automotive
