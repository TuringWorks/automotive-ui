// SignalValidator.cpp
// Signal validation utilities implementation

#include "signal/SignalValidator.h"
#include <cmath>

namespace automotive {
namespace signal {

ValidationRule SignalValidator::rangeRule(double minValue,
                                           double maxValue,
                                           bool clamp)
{
    return [=](const QVariant& value) -> ValidationResult {
        bool ok = false;
        double numValue = value.toDouble(&ok);

        if (!ok) {
            return ValidationResult::fail(
                QStringLiteral("RANGE_INVALID_TYPE"),
                QStringLiteral("Value is not numeric")
            );
        }

        if (numValue < minValue || numValue > maxValue) {
            if (clamp) {
                ValidationResult r;
                r.passed = true;
                r.correctedValue = qBound(minValue, numValue, maxValue);
                return r;
            } else {
                return ValidationResult::fail(
                    QStringLiteral("RANGE_EXCEEDED"),
                    QString::fromLatin1("Value %1 outside range [%2, %3]")
                        .arg(numValue).arg(minValue).arg(maxValue)
                );
            }
        }

        return ValidationResult::pass();
    };
}

ValidationRule SignalValidator::discreteRule(const QVariantList& allowedValues)
{
    return [=](const QVariant& value) -> ValidationResult {
        for (const QVariant& allowed : allowedValues) {
            if (value == allowed) {
                return ValidationResult::pass();
            }
        }

        return ValidationResult::fail(
            QStringLiteral("DISCRETE_INVALID"),
            QStringLiteral("Value not in allowed set")
        );
    };
}

ValidationResult SignalValidator::validateRateOfChange(
    double currentValue,
    double previousValue,
    double maxRatePerSecond,
    qint64 deltaTimeMs)
{
    if (deltaTimeMs <= 0) {
        return ValidationResult::pass();  // Cannot validate without time delta
    }

    double deltaValue = std::abs(currentValue - previousValue);
    double ratePerSecond = (deltaValue * 1000.0) / static_cast<double>(deltaTimeMs);

    if (ratePerSecond > maxRatePerSecond) {
        return ValidationResult::fail(
            QStringLiteral("RATE_EXCEEDED"),
            QString::fromLatin1("Rate of change %1/s exceeds maximum %2/s")
                .arg(ratePerSecond).arg(maxRatePerSecond)
        );
    }

    return ValidationResult::pass();
}

ValidationResult SignalValidator::validateSpeed(double speed, bool isKmh)
{
    // Maximum reasonable speed limits
    const double maxSpeedKmh = 400.0;  // Covers hypercars
    const double maxSpeedMph = 250.0;

    double maxSpeed = isKmh ? maxSpeedKmh : maxSpeedMph;

    if (speed < 0.0) {
        ValidationResult r;
        r.passed = true;
        r.correctedValue = 0.0;
        return r;
    }

    if (speed > maxSpeed) {
        ValidationResult r;
        r.passed = true;
        r.correctedValue = maxSpeed;
        return r;
    }

    return ValidationResult::pass();
}

ValidationResult SignalValidator::validatePercentage(double percent)
{
    if (percent < 0.0 || percent > 100.0) {
        ValidationResult r;
        r.passed = true;
        r.correctedValue = qBound(0.0, percent, 100.0);
        return r;
    }

    return ValidationResult::pass();
}

ValidationResult SignalValidator::validateTemperature(double tempCelsius,
                                                       double minTemp,
                                                       double maxTemp)
{
    if (tempCelsius < minTemp || tempCelsius > maxTemp) {
        return ValidationResult::fail(
            QStringLiteral("TEMP_RANGE_EXCEEDED"),
            QString::fromLatin1("Temperature %1C outside range [%2, %3]")
                .arg(tempCelsius).arg(minTemp).arg(maxTemp)
        );
    }

    return ValidationResult::pass();
}

ValidationResult SignalValidator::validateGear(const QString& gear)
{
    static const QStringList validGears = {
        QStringLiteral("P"), QStringLiteral("R"), QStringLiteral("N"),
        QStringLiteral("D"), QStringLiteral("L"), QStringLiteral("S"),
        QStringLiteral("1"), QStringLiteral("2"), QStringLiteral("3"),
        QStringLiteral("4"), QStringLiteral("5"), QStringLiteral("6"),
        QStringLiteral("7"), QStringLiteral("8"), QStringLiteral("9"),
        QStringLiteral("M"), QStringLiteral("B")  // Manual, Brake regen
    };

    if (validGears.contains(gear.toUpper())) {
        return ValidationResult::pass();
    }

    return ValidationResult::fail(
        QStringLiteral("GEAR_INVALID"),
        QString::fromLatin1("Invalid gear position: %1").arg(gear)
    );
}

ValidationResult SignalValidator::applyRules(const QVariant& value,
                                              const QVector<ValidationRule>& rules)
{
    QVariant currentValue = value;

    for (const ValidationRule& rule : rules) {
        ValidationResult result = rule(currentValue);

        if (!result.passed) {
            return result;
        }

        // Apply corrections if any
        if (result.correctedValue.isValid()) {
            currentValue = result.correctedValue;
        }
    }

    ValidationResult finalResult = ValidationResult::pass();
    if (currentValue != value) {
        finalResult.correctedValue = currentValue;
    }
    return finalResult;
}

} // namespace signal
} // namespace automotive
