// IpcClient.cpp
// IPC client implementation

#include "ipc/IpcClient.h"
#include <QDebug>

namespace automotive {
namespace ipc {

IpcClient::IpcClient(QObject* parent)
    : QObject(parent)
{
    connect(&m_channel, &IpcChannel::stateChanged,
            this, &IpcClient::onChannelStateChanged);
    connect(&m_channel, &IpcChannel::messageReceived,
            this, &IpcClient::messageReceived);
    connect(&m_channel, &IpcChannel::errorOccurred,
            this, &IpcClient::errorOccurred);

    connect(&m_reconnectTimer, &QTimer::timeout,
            this, &IpcClient::onReconnectTimer);
    connect(&m_heartbeatTimer, &QTimer::timeout,
            this, &IpcClient::onHeartbeatTimer);

    m_reconnectTimer.setSingleShot(true);
}

IpcClient::~IpcClient()
{
    disconnect();
}

void IpcClient::connectToServer(const QString& serverName)
{
    m_serverName = serverName;
    m_shouldConnect = true;

    if (m_channel.isConnected()) {
        return;
    }

    m_channel.connectToServer(serverName);
}

void IpcClient::disconnect()
{
    m_shouldConnect = false;
    m_reconnectTimer.stop();
    m_heartbeatTimer.stop();
    m_channel.disconnect();
}

bool IpcClient::isConnected() const
{
    return m_channel.isConnected();
}

bool IpcClient::send(const IpcMessage& message)
{
    return m_channel.send(message);
}

void IpcClient::setReconnectInterval(int intervalMs)
{
    m_reconnectIntervalMs = intervalMs;
}

void IpcClient::setHeartbeatInterval(int intervalMs)
{
    m_heartbeatIntervalMs = intervalMs;
    if (m_heartbeatIntervalMs > 0 && m_channel.isConnected()) {
        m_heartbeatTimer.start(m_heartbeatIntervalMs);
    } else {
        m_heartbeatTimer.stop();
    }
}

void IpcClient::onChannelStateChanged(ChannelState state)
{
    switch (state) {
    case ChannelState::Connected:
        qDebug() << "IpcClient: Connected to" << m_serverName;
        m_reconnectTimer.stop();
        if (m_heartbeatIntervalMs > 0) {
            m_heartbeatTimer.start(m_heartbeatIntervalMs);
        }
        emit connectedChanged(true);
        break;

    case ChannelState::Disconnected:
    case ChannelState::Error:
        qDebug() << "IpcClient: Disconnected from" << m_serverName;
        m_heartbeatTimer.stop();
        emit connectedChanged(false);

        // Schedule reconnection if desired
        if (m_shouldConnect && m_reconnectIntervalMs > 0) {
            m_reconnectTimer.start(m_reconnectIntervalMs);
        }
        break;

    case ChannelState::Connecting:
        // No action needed
        break;
    }
}

void IpcClient::onReconnectTimer()
{
    if (m_shouldConnect && !m_channel.isConnected()) {
        qDebug() << "IpcClient: Attempting reconnection to" << m_serverName;
        m_channel.connectToServer(m_serverName, 1000);
    }
}

void IpcClient::onHeartbeatTimer()
{
    if (m_channel.isConnected()) {
        IpcMessage heartbeat(MessageType::Heartbeat);
        m_channel.send(heartbeat);
    }
}

} // namespace ipc
} // namespace automotive
