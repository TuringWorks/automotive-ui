// IpcChannel.cpp
// IPC channel implementation

#include "ipc/IpcChannel.h"
#include <QDebug>

namespace automotive {
namespace ipc {

IpcChannel::IpcChannel(QObject* parent)
    : QObject(parent)
    , m_socket(new QLocalSocket(this))
    , m_ownsSocket(true)
{
    connect(m_socket, &QLocalSocket::connected,
            this, &IpcChannel::onConnected);
    connect(m_socket, &QLocalSocket::disconnected,
            this, &IpcChannel::onDisconnected);
    connect(m_socket, &QLocalSocket::readyRead,
            this, &IpcChannel::onReadyRead);
    connect(m_socket, &QLocalSocket::errorOccurred,
            this, &IpcChannel::onError);
}

IpcChannel::IpcChannel(QLocalSocket* socket, QObject* parent)
    : QObject(parent)
    , m_socket(socket)
    , m_ownsSocket(false)
{
    if (m_socket) {
        m_socket->setParent(this);

        connect(m_socket, &QLocalSocket::disconnected,
                this, &IpcChannel::onDisconnected);
        connect(m_socket, &QLocalSocket::readyRead,
                this, &IpcChannel::onReadyRead);
        connect(m_socket, &QLocalSocket::errorOccurred,
                this, &IpcChannel::onError);

        if (m_socket->state() == QLocalSocket::ConnectedState) {
            setState(ChannelState::Connected);
        }
    }
}

IpcChannel::~IpcChannel()
{
    if (m_socket && m_state == ChannelState::Connected) {
        m_socket->disconnectFromServer();
    }
}

bool IpcChannel::send(const IpcMessage& message)
{
    if (m_state != ChannelState::Connected || !m_socket) {
        m_lastError = QStringLiteral("Not connected");
        return false;
    }

    QByteArray data = message.serialize();
    qint64 written = m_socket->write(data);

    if (written != data.size()) {
        m_lastError = QStringLiteral("Failed to write complete message");
        return false;
    }

    return true;
}

bool IpcChannel::connectToServer(const QString& serverName, int timeoutMs)
{
    if (m_state == ChannelState::Connected) {
        return true;
    }

    if (!m_socket) {
        m_lastError = QStringLiteral("No socket available");
        return false;
    }

    setState(ChannelState::Connecting);
    m_socket->connectToServer(serverName);

    if (!m_socket->waitForConnected(timeoutMs)) {
        m_lastError = m_socket->errorString();
        setState(ChannelState::Error);
        return false;
    }

    return true;
}

void IpcChannel::disconnect()
{
    if (m_socket) {
        m_socket->disconnectFromServer();
    }
}

void IpcChannel::onConnected()
{
    setState(ChannelState::Connected);
    m_readBuffer.clear();
}

void IpcChannel::onDisconnected()
{
    setState(ChannelState::Disconnected);
    m_readBuffer.clear();
}

void IpcChannel::onReadyRead()
{
    if (!m_socket) return;

    m_readBuffer.append(m_socket->readAll());
    processBuffer();
}

void IpcChannel::onError(QLocalSocket::LocalSocketError error)
{
    Q_UNUSED(error)
    m_lastError = m_socket ? m_socket->errorString() : QStringLiteral("Unknown error");
    setState(ChannelState::Error);
    emit errorOccurred(m_lastError);
}

void IpcChannel::setState(ChannelState state)
{
    if (m_state != state) {
        m_state = state;
        emit stateChanged(state);
    }
}

void IpcChannel::processBuffer()
{
    // Keep processing while we have enough data
    while (m_readBuffer.size() >= static_cast<int>(MessageHeader::SIZE)) {
        // Peek at header to get payload size
        QDataStream stream(m_readBuffer);
        stream.setVersion(QDataStream::Qt_6_0);

        MessageHeader header;
        stream >> header;

        if (!header.isValid()) {
            // Invalid header - try to find next valid magic
            int magicPos = m_readBuffer.indexOf(
                QByteArray::fromRawData(
                    reinterpret_cast<const char*>(&MessageHeader::MAGIC), 4),
                1);

            if (magicPos > 0) {
                emit malformedMessageReceived(
                    QStringLiteral("Discarding %1 bytes of invalid data")
                        .arg(magicPos));
                m_readBuffer.remove(0, magicPos);
            } else {
                // No valid header found, clear buffer
                emit malformedMessageReceived(
                    QStringLiteral("No valid header found, clearing buffer"));
                m_readBuffer.clear();
                return;
            }
            continue;
        }

        // Check if we have complete message
        int totalSize = static_cast<int>(MessageHeader::SIZE + header.payloadSize);
        if (m_readBuffer.size() < totalSize) {
            // Wait for more data
            return;
        }

        // Extract and parse complete message
        QByteArray messageData = m_readBuffer.left(totalSize);
        m_readBuffer.remove(0, totalSize);

        parseMessage(messageData);
    }
}

bool IpcChannel::parseMessage(const QByteArray& data)
{
    bool ok = false;
    IpcMessage message = IpcMessage::deserialize(data, &ok);

    if (!ok) {
        // Security: CR-INF-001 - Log malformed messages
        emit malformedMessageReceived(message.validationError());
        return false;
    }

    emit messageReceived(message);
    return true;
}

} // namespace ipc
} // namespace automotive
