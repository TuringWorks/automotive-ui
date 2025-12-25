// PiiRedactor.h
// PII redaction for logs
// Part of: Shared Platform Layer
// Security: CR-INF-003 - Logging shall avoid sensitive data by default

#ifndef AUTOMOTIVE_PII_REDACTOR_H
#define AUTOMOTIVE_PII_REDACTOR_H

#include <QString>
#include <QRegularExpression>
#include <QVector>

namespace automotive {
namespace logging {

/**
 * @brief PII redaction policy
 */
enum class RedactionPolicy {
    Full,           ///< Replace with [REDACTED]
    Partial,        ///< Keep first/last chars, e.g., j***@example.com
    Hash            ///< Replace with hash for correlation
};

/**
 * @brief PII redaction patterns and utilities
 *
 * Security: CR-INF-003, CR-INF-004 - Prevent PII leakage in logs
 */
class PiiRedactor {
public:
    /**
     * @brief Redact PII from a string
     * @param input Input string
     * @return String with PII redacted
     */
    static QString redact(const QString& input);

    /**
     * @brief Add a custom redaction pattern
     * @param pattern Regular expression pattern
     * @param replacement Replacement string (default: "[REDACTED]")
     */
    static void addPattern(const QRegularExpression& pattern,
                           const QString& replacement = QStringLiteral("[REDACTED]"));

    /**
     * @brief Clear all custom patterns
     */
    static void clearPatterns();

    /**
     * @brief Set redaction policy
     */
    static void setPolicy(RedactionPolicy policy);

    /**
     * @brief Check if a string likely contains PII
     */
    static bool containsPii(const QString& input);

private:
    PiiRedactor() = default;

    static QVector<QPair<QRegularExpression, QString>>& patterns();
    static RedactionPolicy& policy();
    static void initDefaultPatterns();
};

} // namespace logging
} // namespace automotive

#endif // AUTOMOTIVE_PII_REDACTOR_H
