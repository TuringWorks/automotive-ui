// VehicleSignals.cpp
// Vehicle signal definitions and factory implementation

#include "signal/VehicleSignals.h"

namespace automotive {
namespace signal {

void VehicleSignalFactory::registerClusterSignals(SignalHub& hub)
{
    // Speed and motion - Safety critical
    hub.registerSignal(speedSignal(true));
    hub.registerSignal(rpmSignal(8000));

    SignalDefinition odometer;
    odometer.id = QString::fromLatin1(SignalIds::ODOMETER);
    odometer.name = QStringLiteral("Odometer");
    odometer.unit = QStringLiteral("km");
    odometer.minValue = 0.0;
    odometer.maxValue = 9999999.0;
    odometer.defaultValue = 0.0;
    odometer.freshnessMs = 1000;
    odometer.isSafetyCritical = false;
    hub.registerSignal(odometer);

    // Gear - Safety critical
    hub.registerSignal(gearSignal());

    // Energy
    hub.registerSignal(batterySocSignal());

    SignalDefinition batteryRange;
    batteryRange.id = QString::fromLatin1(SignalIds::BATTERY_RANGE);
    batteryRange.name = QStringLiteral("Battery Range");
    batteryRange.unit = QStringLiteral("km");
    batteryRange.minValue = 0.0;
    batteryRange.maxValue = 1000.0;
    batteryRange.defaultValue = 0.0;
    batteryRange.freshnessMs = 5000;
    batteryRange.isSafetyCritical = false;
    hub.registerSignal(batteryRange);

    SignalDefinition powerConsumption;
    powerConsumption.id = QString::fromLatin1(SignalIds::POWER_CONSUMPTION);
    powerConsumption.name = QStringLiteral("Power Consumption");
    powerConsumption.unit = QStringLiteral("kW");
    powerConsumption.minValue = -200.0;  // Regen
    powerConsumption.maxValue = 500.0;
    powerConsumption.defaultValue = 0.0;
    powerConsumption.freshnessMs = 500;
    powerConsumption.isSafetyCritical = false;
    hub.registerSignal(powerConsumption);

    // Critical telltales
    hub.registerSignal(telltaleSignal(
        QString::fromLatin1(SignalIds::TELLTALE_TURN_LEFT),
        QStringLiteral("Turn Left"), false));
    hub.registerSignal(telltaleSignal(
        QString::fromLatin1(SignalIds::TELLTALE_TURN_RIGHT),
        QStringLiteral("Turn Right"), false));
    hub.registerSignal(telltaleSignal(
        QString::fromLatin1(SignalIds::TELLTALE_HAZARD),
        QStringLiteral("Hazard"), true));
    hub.registerSignal(telltaleSignal(
        QString::fromLatin1(SignalIds::TELLTALE_HIGH_BEAM),
        QStringLiteral("High Beam"), false));
    hub.registerSignal(telltaleSignal(
        QString::fromLatin1(SignalIds::TELLTALE_LOW_BEAM),
        QStringLiteral("Low Beam"), false));
    hub.registerSignal(telltaleSignal(
        QString::fromLatin1(SignalIds::TELLTALE_SEATBELT),
        QStringLiteral("Seatbelt"), true));
    hub.registerSignal(telltaleSignal(
        QString::fromLatin1(SignalIds::TELLTALE_DOOR_OPEN),
        QStringLiteral("Door Open"), true));
    hub.registerSignal(telltaleSignal(
        QString::fromLatin1(SignalIds::TELLTALE_ENGINE_CHECK),
        QStringLiteral("Engine Check"), true));
    hub.registerSignal(telltaleSignal(
        QString::fromLatin1(SignalIds::TELLTALE_ABS),
        QStringLiteral("ABS"), true));
    hub.registerSignal(telltaleSignal(
        QString::fromLatin1(SignalIds::TELLTALE_AIRBAG),
        QStringLiteral("Airbag"), true));
    hub.registerSignal(telltaleSignal(
        QString::fromLatin1(SignalIds::TELLTALE_TIRE_PRESSURE),
        QStringLiteral("Tire Pressure"), true));
    hub.registerSignal(telltaleSignal(
        QString::fromLatin1(SignalIds::TELLTALE_BATTERY),
        QStringLiteral("Battery Warning"), true));
    hub.registerSignal(telltaleSignal(
        QString::fromLatin1(SignalIds::TELLTALE_TEMP),
        QStringLiteral("Temperature Warning"), true));

    // ADAS
    SignalDefinition adasEnabled;
    adasEnabled.id = QString::fromLatin1(SignalIds::ADAS_ENABLED);
    adasEnabled.name = QStringLiteral("ADAS Enabled");
    adasEnabled.defaultValue = false;
    adasEnabled.freshnessMs = 500;
    adasEnabled.isSafetyCritical = true;
    hub.registerSignal(adasEnabled);

    SignalDefinition adasActive;
    adasActive.id = QString::fromLatin1(SignalIds::ADAS_ACTIVE);
    adasActive.name = QStringLiteral("ADAS Active");
    adasActive.defaultValue = false;
    adasActive.freshnessMs = 200;
    adasActive.isSafetyCritical = true;
    hub.registerSignal(adasActive);

    // Environment
    SignalDefinition outsideTemp;
    outsideTemp.id = QString::fromLatin1(SignalIds::OUTSIDE_TEMP);
    outsideTemp.name = QStringLiteral("Outside Temperature");
    outsideTemp.unit = QStringLiteral("C");
    outsideTemp.minValue = -50.0;
    outsideTemp.maxValue = 70.0;
    outsideTemp.defaultValue = 20.0;
    outsideTemp.freshnessMs = 10000;
    outsideTemp.isSafetyCritical = false;
    hub.registerSignal(outsideTemp);
}

void VehicleSignalFactory::registerInfotainmentSignals(SignalHub& hub)
{
    // Media signals
    SignalDefinition mediaTitle;
    mediaTitle.id = QString::fromLatin1(SignalIds::MEDIA_TITLE);
    mediaTitle.name = QStringLiteral("Media Title");
    mediaTitle.defaultValue = QString();
    mediaTitle.freshnessMs = 30000;
    hub.registerSignal(mediaTitle);

    SignalDefinition mediaArtist;
    mediaArtist.id = QString::fromLatin1(SignalIds::MEDIA_ARTIST);
    mediaArtist.name = QStringLiteral("Media Artist");
    mediaArtist.defaultValue = QString();
    mediaArtist.freshnessMs = 30000;
    hub.registerSignal(mediaArtist);

    SignalDefinition mediaPlaying;
    mediaPlaying.id = QString::fromLatin1(SignalIds::MEDIA_PLAYING);
    mediaPlaying.name = QStringLiteral("Media Playing");
    mediaPlaying.defaultValue = false;
    mediaPlaying.freshnessMs = 5000;
    hub.registerSignal(mediaPlaying);

    SignalDefinition mediaDuration;
    mediaDuration.id = QString::fromLatin1(SignalIds::MEDIA_DURATION);
    mediaDuration.name = QStringLiteral("Media Duration");
    mediaDuration.unit = QStringLiteral("s");
    mediaDuration.minValue = 0;
    mediaDuration.maxValue = 86400;  // 24 hours
    mediaDuration.defaultValue = 0;
    mediaDuration.freshnessMs = 30000;
    hub.registerSignal(mediaDuration);

    SignalDefinition mediaPosition;
    mediaPosition.id = QString::fromLatin1(SignalIds::MEDIA_POSITION);
    mediaPosition.name = QStringLiteral("Media Position");
    mediaPosition.unit = QStringLiteral("s");
    mediaPosition.minValue = 0;
    mediaPosition.maxValue = 86400;
    mediaPosition.defaultValue = 0;
    mediaPosition.freshnessMs = 1000;
    hub.registerSignal(mediaPosition);

    // Phone signals
    SignalDefinition phoneConnected;
    phoneConnected.id = QString::fromLatin1(SignalIds::PHONE_CONNECTED);
    phoneConnected.name = QStringLiteral("Phone Connected");
    phoneConnected.defaultValue = false;
    phoneConnected.freshnessMs = 5000;
    hub.registerSignal(phoneConnected);

    SignalDefinition phoneCallActive;
    phoneCallActive.id = QString::fromLatin1(SignalIds::PHONE_CALL_ACTIVE);
    phoneCallActive.name = QStringLiteral("Phone Call Active");
    phoneCallActive.defaultValue = false;
    phoneCallActive.freshnessMs = 1000;
    hub.registerSignal(phoneCallActive);

    // Climate signals
    SignalDefinition hvacTempDriver;
    hvacTempDriver.id = QString::fromLatin1(SignalIds::HVAC_TEMP_DRIVER);
    hvacTempDriver.name = QStringLiteral("HVAC Driver Temp");
    hvacTempDriver.unit = QStringLiteral("C");
    hvacTempDriver.minValue = 16.0;
    hvacTempDriver.maxValue = 30.0;
    hvacTempDriver.defaultValue = 22.0;
    hvacTempDriver.freshnessMs = 5000;
    hub.registerSignal(hvacTempDriver);

    SignalDefinition hvacFanSpeed;
    hvacFanSpeed.id = QString::fromLatin1(SignalIds::HVAC_FAN_SPEED);
    hvacFanSpeed.name = QStringLiteral("HVAC Fan Speed");
    hvacFanSpeed.minValue = 0;
    hvacFanSpeed.maxValue = 7;
    hvacFanSpeed.defaultValue = 0;
    hvacFanSpeed.freshnessMs = 5000;
    hub.registerSignal(hvacFanSpeed);

    // Navigation signals
    SignalDefinition navActive;
    navActive.id = QString::fromLatin1(SignalIds::NAV_ACTIVE);
    navActive.name = QStringLiteral("Navigation Active");
    navActive.defaultValue = false;
    navActive.freshnessMs = 5000;
    hub.registerSignal(navActive);

    SignalDefinition navInstruction;
    navInstruction.id = QString::fromLatin1(SignalIds::NAV_NEXT_INSTRUCTION);
    navInstruction.name = QStringLiteral("Next Navigation Instruction");
    navInstruction.defaultValue = QString();
    navInstruction.freshnessMs = 10000;
    hub.registerSignal(navInstruction);

    SignalDefinition navEta;
    navEta.id = QString::fromLatin1(SignalIds::NAV_ETA);
    navEta.name = QStringLiteral("Estimated Time of Arrival");
    navEta.defaultValue = QString();
    navEta.freshnessMs = 30000;
    hub.registerSignal(navEta);
}

SignalDefinition VehicleSignalFactory::speedSignal(bool isKmh)
{
    SignalDefinition def;
    def.id = QString::fromLatin1(SignalIds::VEHICLE_SPEED);
    def.name = QStringLiteral("Vehicle Speed");
    def.unit = isKmh ? QStringLiteral("km/h") : QStringLiteral("mph");
    def.minValue = 0.0;
    def.maxValue = isKmh ? 400.0 : 250.0;
    def.defaultValue = 0.0;
    def.freshnessMs = 300;  // SR-CL-001: stale within 300ms
    def.maxRateOfChange = isKmh ? 50.0 : 30.0;  // Max 50 km/h per second change
    def.isSafetyCritical = true;
    return def;
}

SignalDefinition VehicleSignalFactory::rpmSignal(int maxRpm)
{
    SignalDefinition def;
    def.id = QString::fromLatin1(SignalIds::ENGINE_RPM);
    def.name = QStringLiteral("Engine RPM");
    def.unit = QStringLiteral("rpm");
    def.minValue = 0;
    def.maxValue = maxRpm;
    def.defaultValue = 0;
    def.freshnessMs = 200;
    def.maxRateOfChange = 5000.0;  // Max 5000 RPM per second change
    def.isSafetyCritical = false;
    return def;
}

SignalDefinition VehicleSignalFactory::batterySocSignal()
{
    SignalDefinition def;
    def.id = QString::fromLatin1(SignalIds::BATTERY_SOC);
    def.name = QStringLiteral("Battery State of Charge");
    def.unit = QStringLiteral("%");
    def.minValue = 0.0;
    def.maxValue = 100.0;
    def.defaultValue = 0.0;
    def.freshnessMs = 5000;
    def.isSafetyCritical = false;
    return def;
}

SignalDefinition VehicleSignalFactory::gearSignal()
{
    SignalDefinition def;
    def.id = QString::fromLatin1(SignalIds::GEAR_POSITION);
    def.name = QStringLiteral("Gear Position");
    def.defaultValue = QStringLiteral("P");
    def.freshnessMs = 500;
    def.isSafetyCritical = true;
    return def;
}

SignalDefinition VehicleSignalFactory::telltaleSignal(const QString& id,
                                                       const QString& name,
                                                       bool isCritical)
{
    SignalDefinition def;
    def.id = id;
    def.name = name;
    def.defaultValue = false;
    def.freshnessMs = isCritical ? 500 : 1000;
    def.isSafetyCritical = isCritical;
    return def;
}

} // namespace signal
} // namespace automotive
