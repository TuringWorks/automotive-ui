// SignalValidator.h
// Signal validation utilities
// Part of: Shared Platform Layer

#ifndef AUTOMOTIVE_SIGNAL_VALIDATOR_H
#define AUTOMOTIVE_SIGNAL_VALIDATOR_H

#include <QVariant>
#include <QString>
#include <functional>

namespace automotive {
namespace signal {

/**
 * @brief Signal validation result
 */
struct ValidationResult {
    bool passed{false};
    QString errorCode;
    QString errorMessage;
    QVariant correctedValue;  ///< Clamped/corrected value if applicable

    static ValidationResult pass() {
        ValidationResult r;
        r.passed = true;
        return r;
    }

    static ValidationResult fail(const QString& code, const QString& msg) {
        ValidationResult r;
        r.passed = false;
        r.errorCode = code;
        r.errorMessage = msg;
        return r;
    }
};

/**
 * @brief Validation rule type
 */
using ValidationRule = std::function<ValidationResult(const QVariant&)>;

/**
 * @brief Signal validation utilities
 *
 * Provides common validation functions for vehicle signals.
 * All functions are deterministic with bounded execution time.
 */
class SignalValidator {
public:
    /**
     * @brief Create a range validation rule
     * @param minValue Minimum allowed value
     * @param maxValue Maximum allowed value
     * @param clamp If true, clamp out-of-range values instead of failing
     */
    static ValidationRule rangeRule(double minValue,
                                     double maxValue,
                                     bool clamp = true);

    /**
     * @brief Create a discrete value validation rule
     * @param allowedValues List of allowed values
     */
    static ValidationRule discreteRule(const QVariantList& allowedValues);

    /**
     * @brief Create a rate-of-change validation rule
     * @param maxRatePerSecond Maximum allowed change per second
     * @param previousValue Previous value for comparison
     * @param deltaTimeMs Time since previous value in milliseconds
     */
    static ValidationResult validateRateOfChange(
        double currentValue,
        double previousValue,
        double maxRatePerSecond,
        qint64 deltaTimeMs);

    /**
     * @brief Validate speed signal (km/h or mph)
     * @param speed Speed value
     * @param isKmh true if km/h, false if mph
     */
    static ValidationResult validateSpeed(double speed, bool isKmh = true);

    /**
     * @brief Validate battery/fuel percentage
     * @param percent Percentage value (0-100)
     */
    static ValidationResult validatePercentage(double percent);

    /**
     * @brief Validate temperature value
     * @param tempCelsius Temperature in Celsius
     * @param minTemp Minimum valid temperature
     * @param maxTemp Maximum valid temperature
     */
    static ValidationResult validateTemperature(double tempCelsius,
                                                 double minTemp = -50.0,
                                                 double maxTemp = 150.0);

    /**
     * @brief Validate gear position
     * @param gear Gear string (P, R, N, D, 1, 2, etc.)
     */
    static ValidationResult validateGear(const QString& gear);

    /**
     * @brief Apply a list of validation rules
     * @param value Value to validate
     * @param rules List of validation rules
     * @return First failing result, or pass if all succeed
     */
    static ValidationResult applyRules(const QVariant& value,
                                        const QVector<ValidationRule>& rules);

private:
    SignalValidator() = default;  // Static-only class
};

} // namespace signal
} // namespace automotive

#endif // AUTOMOTIVE_SIGNAL_VALIDATOR_H
