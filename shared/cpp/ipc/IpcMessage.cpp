// IpcMessage.cpp
// IPC message implementation

#include "ipc/IpcMessage.h"
#include <QCryptographicHash>
#include <QDateTime>
#include <QIODevice>

namespace automotive {
namespace ipc {

uint32_t IpcMessage::s_sequenceCounter = 0;

IpcMessage::IpcMessage()
{
    m_valid = false;
}

IpcMessage::IpcMessage(MessageType type)
{
    m_header.type = type;
    m_header.sequenceNumber = nextSequenceNumber();
    m_header.timestamp = static_cast<uint64_t>(
        QDateTime::currentMSecsSinceEpoch());
    m_valid = true;
}

IpcMessage::IpcMessage(MessageType type, const QVariantMap& payload)
    : IpcMessage(type)
{
    m_payload = payload;
}

void IpcMessage::setValue(const QString& key, const QVariant& value)
{
    m_payload.insert(key, value);
}

void IpcMessage::setPayload(const QVariantMap& payload)
{
    m_payload = payload;
}

void IpcMessage::setSequenceNumber(uint32_t seq)
{
    m_header.sequenceNumber = seq;
}

uint32_t IpcMessage::nextSequenceNumber()
{
    return ++s_sequenceCounter;
}

QByteArray IpcMessage::serialize() const
{
    // Serialize payload first
    QByteArray payloadData;
    {
        QDataStream payloadStream(&payloadData, QIODevice::WriteOnly);
        payloadStream.setVersion(QDataStream::Qt_6_0);
        payloadStream << m_payload;
    }

    // Build complete message
    QByteArray result;
    result.reserve(static_cast<int>(MessageHeader::SIZE) + payloadData.size());

    MessageHeader header = m_header;
    header.payloadSize = static_cast<uint32_t>(payloadData.size());
    header.checksum = calculateChecksum(payloadData);

    QDataStream stream(&result, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_6_0);
    stream << header;

    result.append(payloadData);

    return result;
}

IpcMessage IpcMessage::deserialize(const QByteArray& data, bool* ok)
{
    IpcMessage msg;

    if (ok) *ok = false;

    if (data.size() < static_cast<int>(MessageHeader::SIZE)) {
        msg.m_validationError = QStringLiteral("Data too small for header");
        return msg;
    }

    QDataStream stream(data);
    stream.setVersion(QDataStream::Qt_6_0);

    // Read header
    stream >> msg.m_header;

    if (!msg.m_header.isValid()) {
        msg.m_validationError = QStringLiteral("Invalid message header (magic/version)");
        return msg;
    }

    // Validate payload size
    int expectedSize = static_cast<int>(MessageHeader::SIZE + msg.m_header.payloadSize);
    if (data.size() < expectedSize) {
        msg.m_validationError = QStringLiteral("Data too small for payload");
        return msg;
    }

    // Extract and validate payload
    QByteArray payloadData = data.mid(static_cast<int>(MessageHeader::SIZE),
                                       static_cast<int>(msg.m_header.payloadSize));

    uint32_t expectedChecksum = calculateChecksum(payloadData);
    if (msg.m_header.checksum != expectedChecksum) {
        msg.m_validationError = QStringLiteral("Checksum mismatch - message corrupted");
        return msg;
    }

    // Deserialize payload
    QDataStream payloadStream(payloadData);
    payloadStream.setVersion(QDataStream::Qt_6_0);
    payloadStream >> msg.m_payload;

    if (payloadStream.status() != QDataStream::Ok) {
        msg.m_validationError = QStringLiteral("Payload deserialization failed");
        return msg;
    }

    msg.m_valid = true;
    if (ok) *ok = true;

    return msg;
}

bool IpcMessage::validateChecksum() const
{
    QByteArray payloadData;
    {
        QDataStream payloadStream(&payloadData, QIODevice::WriteOnly);
        payloadStream.setVersion(QDataStream::Qt_6_0);
        payloadStream << m_payload;
    }

    return m_header.checksum == calculateChecksum(payloadData);
}

uint32_t IpcMessage::calculateChecksum(const QByteArray& data)
{
    // Use CRC32-style checksum (simplified for demonstration)
    // In production, use a proper CRC32 implementation
    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Md5);

    // Take first 4 bytes as checksum
    uint32_t checksum = 0;
    if (hash.size() >= 4) {
        checksum = static_cast<uint32_t>(
            (static_cast<uint8_t>(hash[0]) << 24) |
            (static_cast<uint8_t>(hash[1]) << 16) |
            (static_cast<uint8_t>(hash[2]) << 8) |
            static_cast<uint8_t>(hash[3])
        );
    }
    return checksum;
}

void IpcMessage::updateChecksum()
{
    QByteArray payloadData;
    {
        QDataStream payloadStream(&payloadData, QIODevice::WriteOnly);
        payloadStream.setVersion(QDataStream::Qt_6_0);
        payloadStream << m_payload;
    }
    m_header.checksum = calculateChecksum(payloadData);
}

QDataStream& operator<<(QDataStream& stream, const MessageHeader& header)
{
    stream << header.magic;
    stream << header.version;
    stream << static_cast<uint16_t>(header.type);
    stream << header.payloadSize;
    stream << header.sequenceNumber;
    stream << header.timestamp;
    stream << header.checksum;
    return stream;
}

QDataStream& operator>>(QDataStream& stream, MessageHeader& header)
{
    uint16_t typeValue;
    stream >> header.magic;
    stream >> header.version;
    stream >> typeValue;
    header.type = static_cast<MessageType>(typeValue);
    stream >> header.payloadSize;
    stream >> header.sequenceNumber;
    stream >> header.timestamp;
    stream >> header.checksum;
    return stream;
}

} // namespace ipc
} // namespace automotive
