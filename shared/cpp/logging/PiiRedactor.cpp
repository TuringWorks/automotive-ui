// PiiRedactor.cpp
// PII redaction implementation

#include "logging/PiiRedactor.h"
#include <QCryptographicHash>

namespace automotive {
namespace logging {

QVector<QPair<QRegularExpression, QString>>& PiiRedactor::patterns()
{
    static QVector<QPair<QRegularExpression, QString>> s_patterns;
    static bool initialized = false;

    if (!initialized) {
        initialized = true;  // Set BEFORE calling to prevent recursion
        initDefaultPatterns();
    }

    return s_patterns;
}

RedactionPolicy& PiiRedactor::policy()
{
    static RedactionPolicy s_policy = RedactionPolicy::Full;
    return s_policy;
}

void PiiRedactor::initDefaultPatterns()
{
    auto& p = patterns();

    // Email addresses
    p.append({
        QRegularExpression(QStringLiteral(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})")),
        QStringLiteral("[EMAIL]")
    });

    // Phone numbers (various formats)
    p.append({
        QRegularExpression(QStringLiteral(R"(\+?[0-9]{1,4}[-.\s]?\(?[0-9]{1,4}\)?[-.\s]?[0-9]{1,4}[-.\s]?[0-9]{1,9})")),
        QStringLiteral("[PHONE]")
    });

    // Credit card numbers (basic pattern)
    p.append({
        QRegularExpression(QStringLiteral(R"(\b[0-9]{4}[-\s]?[0-9]{4}[-\s]?[0-9]{4}[-\s]?[0-9]{4}\b)")),
        QStringLiteral("[CARD]")
    });

    // SSN (US format)
    p.append({
        QRegularExpression(QStringLiteral(R"(\b[0-9]{3}-[0-9]{2}-[0-9]{4}\b)")),
        QStringLiteral("[SSN]")
    });

    // IP addresses
    p.append({
        QRegularExpression(QStringLiteral(R"(\b(?:[0-9]{1,3}\.){3}[0-9]{1,3}\b)")),
        QStringLiteral("[IP]")
    });

    // MAC addresses
    p.append({
        QRegularExpression(QStringLiteral(R"(\b(?:[0-9A-Fa-f]{2}[:-]){5}[0-9A-Fa-f]{2}\b)")),
        QStringLiteral("[MAC]")
    });

    // VIN (Vehicle Identification Number)
    p.append({
        QRegularExpression(QStringLiteral(R"(\b[A-HJ-NPR-Z0-9]{17}\b)")),
        QStringLiteral("[VIN]")
    });

    // License plates (generic pattern)
    p.append({
        QRegularExpression(QStringLiteral(R"(\b[A-Z]{1,3}[-\s]?[0-9]{1,4}[-\s]?[A-Z]{0,3}\b)")),
        QStringLiteral("[PLATE]")
    });

    // Coordinates (lat/long)
    p.append({
        QRegularExpression(QStringLiteral(R"([-+]?([1-8]?\d(\.\d+)?|90(\.0+)?),\s*[-+]?(180(\.0+)?|((1[0-7]\d)|([1-9]?\d))(\.\d+)?))")),
        QStringLiteral("[COORDS]")
    });

    // API keys / tokens (generic pattern for hex strings)
    p.append({
        QRegularExpression(QStringLiteral(R"(\b[a-fA-F0-9]{32,64}\b)")),
        QStringLiteral("[TOKEN]")
    });
}

QString PiiRedactor::redact(const QString& input)
{
    QString result = input;

    for (const auto& pattern : patterns()) {
        switch (policy()) {
        case RedactionPolicy::Full:
            result.replace(pattern.first, pattern.second);
            break;

        case RedactionPolicy::Partial: {
            QRegularExpressionMatchIterator it = pattern.first.globalMatch(result);
            while (it.hasNext()) {
                QRegularExpressionMatch match = it.next();
                QString matched = match.captured();
                QString partial;

                if (matched.length() > 4) {
                    partial = matched.left(2) +
                              QString(matched.length() - 4, QLatin1Char('*')) +
                              matched.right(2);
                } else {
                    partial = QString(matched.length(), QLatin1Char('*'));
                }

                result.replace(match.capturedStart(), match.capturedLength(), partial);
            }
            break;
        }

        case RedactionPolicy::Hash: {
            QRegularExpressionMatchIterator it = pattern.first.globalMatch(result);
            while (it.hasNext()) {
                QRegularExpressionMatch match = it.next();
                QString matched = match.captured();
                QByteArray hash = QCryptographicHash::hash(
                    matched.toUtf8(), QCryptographicHash::Sha256);
                QString hashStr = QString::fromLatin1("[H:%1]")
                    .arg(QString::fromLatin1(hash.toHex().left(8)));
                result.replace(match.capturedStart(), match.capturedLength(), hashStr);
            }
            break;
        }
        }
    }

    return result;
}

void PiiRedactor::addPattern(const QRegularExpression& pattern,
                              const QString& replacement)
{
    patterns().append({pattern, replacement});
}

void PiiRedactor::clearPatterns()
{
    patterns().clear();
}

void PiiRedactor::setPolicy(RedactionPolicy p)
{
    policy() = p;
}

bool PiiRedactor::containsPii(const QString& input)
{
    for (const auto& pattern : patterns()) {
        if (pattern.first.match(input).hasMatch()) {
            return true;
        }
    }
    return false;
}

} // namespace logging
} // namespace automotive
