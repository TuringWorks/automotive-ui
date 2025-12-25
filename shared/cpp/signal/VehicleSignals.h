// VehicleSignals.h
// Vehicle signal definitions and constants
// Part of: Shared Platform Layer

#ifndef AUTOMOTIVE_VEHICLE_SIGNALS_H
#define AUTOMOTIVE_VEHICLE_SIGNALS_H

#include "signal/SignalHub.h"
#include <QString>

namespace automotive {
namespace signal {

/**
 * @brief Standard vehicle signal identifiers
 *
 * These IDs are used consistently across the codebase for signal identification.
 */
namespace SignalIds {
    // Speed and motion
    constexpr const char* VEHICLE_SPEED = "vehicle.speed";
    constexpr const char* VEHICLE_SPEED_UNIT = "vehicle.speed.unit";  // "kmh" or "mph"
    constexpr const char* ENGINE_RPM = "engine.rpm";
    constexpr const char* ODOMETER = "vehicle.odometer";
    constexpr const char* TRIP_ODOMETER = "vehicle.trip_odometer";

    // Powertrain
    constexpr const char* GEAR_POSITION = "powertrain.gear";
    constexpr const char* DRIVE_MODE = "powertrain.drive_mode";
    constexpr const char* THROTTLE_POSITION = "powertrain.throttle";
    constexpr const char* BRAKE_PRESSURE = "powertrain.brake_pressure";
    constexpr const char* PARKING_BRAKE = "powertrain.parking_brake";

    // Energy
    constexpr const char* FUEL_LEVEL = "energy.fuel_level";
    constexpr const char* BATTERY_SOC = "energy.battery_soc";  // State of charge
    constexpr const char* BATTERY_RANGE = "energy.battery_range";
    constexpr const char* FUEL_RANGE = "energy.fuel_range";
    constexpr const char* CHARGING_STATE = "energy.charging_state";
    constexpr const char* POWER_CONSUMPTION = "energy.power_consumption";
    constexpr const char* REGEN_LEVEL = "energy.regen_level";

    // Telltales (boolean)
    constexpr const char* TELLTALE_TURN_LEFT = "telltale.turn_left";
    constexpr const char* TELLTALE_TURN_RIGHT = "telltale.turn_right";
    constexpr const char* TELLTALE_HAZARD = "telltale.hazard";
    constexpr const char* TELLTALE_HIGH_BEAM = "telltale.high_beam";
    constexpr const char* TELLTALE_LOW_BEAM = "telltale.low_beam";
    constexpr const char* TELLTALE_FOG_FRONT = "telltale.fog_front";
    constexpr const char* TELLTALE_FOG_REAR = "telltale.fog_rear";
    constexpr const char* TELLTALE_SEATBELT = "telltale.seatbelt";
    constexpr const char* TELLTALE_DOOR_OPEN = "telltale.door_open";
    constexpr const char* TELLTALE_TRUNK_OPEN = "telltale.trunk_open";
    constexpr const char* TELLTALE_ENGINE_CHECK = "telltale.engine_check";
    constexpr const char* TELLTALE_ABS = "telltale.abs";
    constexpr const char* TELLTALE_AIRBAG = "telltale.airbag";
    constexpr const char* TELLTALE_TIRE_PRESSURE = "telltale.tire_pressure";
    constexpr const char* TELLTALE_OIL_PRESSURE = "telltale.oil_pressure";
    constexpr const char* TELLTALE_BATTERY = "telltale.battery_warning";
    constexpr const char* TELLTALE_TEMP = "telltale.temperature";

    // ADAS
    constexpr const char* ADAS_ENABLED = "adas.enabled";
    constexpr const char* ADAS_ACTIVE = "adas.active";
    constexpr const char* ADAS_MODE = "adas.mode";
    constexpr const char* ADAS_SET_SPEED = "adas.set_speed";
    constexpr const char* ADAS_LANE_DEPARTURE = "adas.lane_departure";
    constexpr const char* ADAS_COLLISION_WARNING = "adas.collision_warning";

    // Environment
    constexpr const char* OUTSIDE_TEMP = "environment.outside_temp";
    constexpr const char* TIME = "environment.time";
    constexpr const char* DATE = "environment.date";

    // Climate
    constexpr const char* HVAC_TEMP_DRIVER = "climate.temp_driver";
    constexpr const char* HVAC_TEMP_PASSENGER = "climate.temp_passenger";
    constexpr const char* HVAC_FAN_SPEED = "climate.fan_speed";
    constexpr const char* HVAC_AC_ON = "climate.ac_on";
    constexpr const char* HVAC_RECIRC = "climate.recirculation";

    // Media (infotainment)
    constexpr const char* MEDIA_TITLE = "media.title";
    constexpr const char* MEDIA_ARTIST = "media.artist";
    constexpr const char* MEDIA_ALBUM = "media.album";
    constexpr const char* MEDIA_DURATION = "media.duration";
    constexpr const char* MEDIA_POSITION = "media.position";
    constexpr const char* MEDIA_PLAYING = "media.playing";
    constexpr const char* MEDIA_SOURCE = "media.source";

    // Phone
    constexpr const char* PHONE_CONNECTED = "phone.connected";
    constexpr const char* PHONE_CALL_ACTIVE = "phone.call_active";
    constexpr const char* PHONE_CALLER_NAME = "phone.caller_name";

    // Navigation
    constexpr const char* NAV_ACTIVE = "nav.active";
    constexpr const char* NAV_NEXT_INSTRUCTION = "nav.next_instruction";
    constexpr const char* NAV_DISTANCE_TO_TURN = "nav.distance_to_turn";
    constexpr const char* NAV_ETA = "nav.eta";
}

/**
 * @brief Factory for creating standard vehicle signal definitions
 */
class VehicleSignalFactory {
public:
    /**
     * @brief Register all standard cluster signals with the hub
     * @param hub Signal hub to register with
     */
    static void registerClusterSignals(SignalHub& hub);

    /**
     * @brief Register all infotainment signals with the hub
     * @param hub Signal hub to register with
     */
    static void registerInfotainmentSignals(SignalHub& hub);

    /**
     * @brief Create speed signal definition
     * @param isKmh true for km/h, false for mph
     */
    static SignalDefinition speedSignal(bool isKmh = true);

    /**
     * @brief Create RPM signal definition
     * @param maxRpm Maximum RPM for the vehicle
     */
    static SignalDefinition rpmSignal(int maxRpm = 8000);

    /**
     * @brief Create battery SOC signal definition
     */
    static SignalDefinition batterySocSignal();

    /**
     * @brief Create gear position signal definition
     */
    static SignalDefinition gearSignal();

    /**
     * @brief Create boolean telltale signal definition
     * @param id Signal identifier
     * @param name Human-readable name
     * @param isCritical true if this is a critical warning telltale
     */
    static SignalDefinition telltaleSignal(const QString& id,
                                            const QString& name,
                                            bool isCritical = false);

private:
    VehicleSignalFactory() = default;
};

} // namespace signal
} // namespace automotive

#endif // AUTOMOTIVE_VEHICLE_SIGNALS_H
