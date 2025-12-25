// TelltaleManager.cpp
// Telltale manager implementation

#include "TelltaleManager.h"
#include "signal/VehicleSignals.h"

namespace automotive {
namespace driver {

TelltaleManager::TelltaleManager(signal::SignalHub* signalHub, QObject* parent)
    : QObject(parent)
    , m_signalHub(signalHub)
{
    Q_ASSERT(signalHub != nullptr);

    connect(m_signalHub, &signal::SignalHub::signalUpdated,
            this, &TelltaleManager::onSignalUpdated);
}

TelltaleManager::~TelltaleManager() = default;

void TelltaleManager::registerTelltale(const QString& signalId,
                                        const QString& name,
                                        const QString& iconPath,
                                        int priority)
{
    TelltaleConfig config;
    config.name = name;
    config.iconPath = iconPath;
    config.priority = priority;
    config.active = false;
    config.valid = true;

    m_telltales.insert(signalId, config);
}

QVector<TelltaleState> TelltaleManager::activeTelltales() const
{
    QVector<TelltaleState> result;

    for (auto it = m_telltales.constBegin(); it != m_telltales.constEnd(); ++it) {
        if (it.value().active) {
            result.append(toState(it.key()));
        }
    }

    // Sort by priority
    std::sort(result.begin(), result.end(),
        [](const TelltaleState& a, const TelltaleState& b) {
            return a.priority < b.priority;
        }
    );

    return result;
}

QVariantList TelltaleManager::activeTelltalesToVariant() const
{
    QVariantList result;
    for (const TelltaleState& state : activeTelltales()) {
        result.append(stateToVariant(state));
    }
    return result;
}

QVector<TelltaleState> TelltaleManager::criticalTelltales() const
{
    QVector<TelltaleState> result;

    for (auto it = m_telltales.constBegin(); it != m_telltales.constEnd(); ++it) {
        if (it.value().active && it.value().priority <= 1) {  // Critical or Warning
            result.append(toState(it.key()));
        }
    }

    std::sort(result.begin(), result.end(),
        [](const TelltaleState& a, const TelltaleState& b) {
            return a.priority < b.priority;
        }
    );

    return result;
}

QVariantList TelltaleManager::criticalTelltalesToVariant() const
{
    QVariantList result;
    for (const TelltaleState& state : criticalTelltales()) {
        result.append(stateToVariant(state));
    }
    return result;
}

QVariantMap TelltaleManager::getTelltale(const QString& id) const
{
    if (m_telltales.contains(id)) {
        return stateToVariant(toState(id));
    }
    return QVariantMap();
}

int TelltaleManager::activeCount() const
{
    return m_activeCount;
}

bool TelltaleManager::hasCritical() const
{
    return m_hasCritical;
}

void TelltaleManager::initializeDefaults()
{
    using namespace signal;

    // Turn signals
    registerTelltale(QString::fromLatin1(SignalIds::TELLTALE_TURN_LEFT),
                     QStringLiteral("Turn Left"), QStringLiteral("qrc:/icons/turn_left.svg"), 3);
    registerTelltale(QString::fromLatin1(SignalIds::TELLTALE_TURN_RIGHT),
                     QStringLiteral("Turn Right"), QStringLiteral("qrc:/icons/turn_right.svg"), 3);

    // Lights
    registerTelltale(QString::fromLatin1(SignalIds::TELLTALE_HIGH_BEAM),
                     QStringLiteral("High Beam"), QStringLiteral("qrc:/icons/high_beam.svg"), 3);
    registerTelltale(QString::fromLatin1(SignalIds::TELLTALE_LOW_BEAM),
                     QStringLiteral("Low Beam"), QStringLiteral("qrc:/icons/low_beam.svg"), 3);

    // Critical warnings (priority 0)
    registerTelltale(QString::fromLatin1(SignalIds::TELLTALE_AIRBAG),
                     QStringLiteral("Airbag"), QStringLiteral("qrc:/icons/airbag.svg"), 0);
    registerTelltale(QString::fromLatin1(SignalIds::TELLTALE_ENGINE_CHECK),
                     QStringLiteral("Engine Check"), QStringLiteral("qrc:/icons/engine.svg"), 0);

    // Warning (priority 1)
    registerTelltale(QString::fromLatin1(SignalIds::TELLTALE_SEATBELT),
                     QStringLiteral("Seatbelt"), QStringLiteral("qrc:/icons/seatbelt.svg"), 1);
    registerTelltale(QString::fromLatin1(SignalIds::TELLTALE_DOOR_OPEN),
                     QStringLiteral("Door Open"), QStringLiteral("qrc:/icons/door.svg"), 1);
    registerTelltale(QString::fromLatin1(SignalIds::TELLTALE_ABS),
                     QStringLiteral("ABS"), QStringLiteral("qrc:/icons/abs.svg"), 1);
    registerTelltale(QString::fromLatin1(SignalIds::TELLTALE_TIRE_PRESSURE),
                     QStringLiteral("Tire Pressure"), QStringLiteral("qrc:/icons/tire.svg"), 1);
    registerTelltale(QString::fromLatin1(SignalIds::TELLTALE_BATTERY),
                     QStringLiteral("Battery"), QStringLiteral("qrc:/icons/battery_warning.svg"), 1);
    registerTelltale(QString::fromLatin1(SignalIds::TELLTALE_TEMP),
                     QStringLiteral("Temperature"), QStringLiteral("qrc:/icons/temp.svg"), 1);

    // Hazard
    registerTelltale(QString::fromLatin1(SignalIds::TELLTALE_HAZARD),
                     QStringLiteral("Hazard"), QStringLiteral("qrc:/icons/hazard.svg"), 1);
}

void TelltaleManager::onSignalUpdated(const QString& signalId,
                                       const signal::SignalValue& value)
{
    if (!m_telltales.contains(signalId)) {
        return;
    }

    TelltaleConfig& config = m_telltales[signalId];
    bool newActive = value.value.toBool();
    bool newValid = value.isValid();

    if (newActive != config.active || newValid != config.valid) {
        bool wasActive = config.active;
        int oldActiveCount = m_activeCount;
        bool hadCritical = m_hasCritical;

        config.active = newActive;
        config.valid = newValid;

        // Update counts
        if (newActive && !wasActive) {
            m_activeCount++;
            emit telltaleActivated(signalId, config.priority);
        } else if (!newActive && wasActive) {
            m_activeCount--;
            emit telltaleDeactivated(signalId);
        }

        // Update critical status
        m_hasCritical = false;
        for (auto it = m_telltales.constBegin(); it != m_telltales.constEnd(); ++it) {
            if (it.value().active && it.value().priority == 0) {
                m_hasCritical = true;
                break;
            }
        }

        // Emit signals
        if (m_activeCount != oldActiveCount) {
            emit activeCountChanged(m_activeCount);
        }
        if (m_hasCritical != hadCritical) {
            emit hasCriticalChanged(m_hasCritical);
        }

        emit activeTelltalesChanged();
        if (config.priority <= 1) {
            emit criticalTelltalesChanged();
        }
    }
}

TelltaleState TelltaleManager::toState(const QString& signalId) const
{
    TelltaleState state;
    state.id = signalId;

    if (m_telltales.contains(signalId)) {
        const TelltaleConfig& config = m_telltales[signalId];
        state.name = config.name;
        state.iconPath = config.iconPath;
        state.priority = config.priority;
        state.active = config.active;
        state.valid = config.valid;
        // Turn signals (priority 3) and critical warnings (priority 0-1) should blink
        state.blinking = (config.priority <= 1 || config.priority == 3) && config.active;
    }

    return state;
}

QVariantMap TelltaleManager::stateToVariant(const TelltaleState& state) const
{
    QVariantMap map;
    map[QStringLiteral("id")] = state.id;
    map[QStringLiteral("name")] = state.name;
    map[QStringLiteral("iconPath")] = state.iconPath;
    map[QStringLiteral("priority")] = state.priority;
    map[QStringLiteral("active")] = state.active;
    map[QStringLiteral("valid")] = state.valid;
    map[QStringLiteral("blinking")] = state.blinking;
    return map;
}

} // namespace driver
} // namespace automotive
