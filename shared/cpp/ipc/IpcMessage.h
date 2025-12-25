// IpcMessage.h
// IPC message definitions and serialization
// Part of: Shared Platform Layer
// Security: CR-INF-001 - Messages include versioning and integrity checks

#ifndef AUTOMOTIVE_IPC_MESSAGE_H
#define AUTOMOTIVE_IPC_MESSAGE_H

#include <QByteArray>
#include <QVariantMap>
#include <QString>
#include <QDataStream>
#include <cstdint>

namespace automotive {
namespace ipc {

/**
 * @brief IPC message types
 */
enum class MessageType : uint16_t {
    Invalid = 0,
    Heartbeat = 1,
    SignalUpdate = 10,
    SignalBatch = 11,
    AlertNotify = 20,
    AlertAck = 21,
    ThemeChange = 30,
    LanguageChange = 31,
    TimeSync = 40,
    SettingsRequest = 50,
    SettingsResponse = 51,
    PermissionRequest = 60,
    PermissionResponse = 61,
    AuditEvent = 100,
    Error = 255
};

/**
 * @brief IPC message header
 *
 * Fixed-size header for all IPC messages.
 * Security: Includes version and checksum for integrity (CR-INF-001)
 */
struct MessageHeader {
    static constexpr uint32_t MAGIC = 0x41555449;  // "AUTI"
    static constexpr uint16_t VERSION = 1;

    uint32_t magic{MAGIC};
    uint16_t version{VERSION};
    MessageType type{MessageType::Invalid};
    uint32_t payloadSize{0};
    uint32_t sequenceNumber{0};
    uint64_t timestamp{0};
    uint32_t checksum{0};  // CRC32 of payload

    bool isValid() const {
        return magic == MAGIC && version == VERSION;
    }

    static constexpr size_t SIZE = 28;  // Fixed header size in bytes
};

/**
 * @brief IPC message container
 *
 * Encapsulates a complete IPC message with header and payload.
 * Security: All messages are validated before processing (CR-INF-001)
 */
class IpcMessage {
public:
    IpcMessage();
    explicit IpcMessage(MessageType type);
    IpcMessage(MessageType type, const QVariantMap& payload);

    // Header access
    MessageType type() const { return m_header.type; }
    uint32_t sequenceNumber() const { return m_header.sequenceNumber; }
    uint64_t timestamp() const { return m_header.timestamp; }
    bool isValid() const { return m_valid; }

    // Payload access
    const QVariantMap& payload() const { return m_payload; }
    QVariant value(const QString& key) const { return m_payload.value(key); }
    void setValue(const QString& key, const QVariant& value);
    void setPayload(const QVariantMap& payload);

    // Serialization
    QByteArray serialize() const;
    static IpcMessage deserialize(const QByteArray& data, bool* ok = nullptr);

    // Sequence number management
    void setSequenceNumber(uint32_t seq);
    static uint32_t nextSequenceNumber();

    // Validation
    bool validateChecksum() const;
    QString validationError() const { return m_validationError; }

private:
    static uint32_t calculateChecksum(const QByteArray& data);
    void updateChecksum();

    MessageHeader m_header;
    QVariantMap m_payload;
    bool m_valid{false};
    QString m_validationError;

    static uint32_t s_sequenceCounter;
};

/**
 * @brief Serialize message header to data stream
 */
QDataStream& operator<<(QDataStream& stream, const MessageHeader& header);

/**
 * @brief Deserialize message header from data stream
 */
QDataStream& operator>>(QDataStream& stream, MessageHeader& header);

} // namespace ipc
} // namespace automotive

#endif // AUTOMOTIVE_IPC_MESSAGE_H
