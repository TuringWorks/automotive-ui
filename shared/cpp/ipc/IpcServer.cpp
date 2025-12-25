// IpcServer.cpp
// IPC server implementation

#include "ipc/IpcServer.h"
#include <QDebug>

namespace automotive {
namespace ipc {

IpcServer::IpcServer(QObject* parent)
    : QObject(parent)
{
    connect(&m_server, &QLocalServer::newConnection,
            this, &IpcServer::onNewConnection);
}

IpcServer::~IpcServer()
{
    close();
}

bool IpcServer::listen(const QString& serverName)
{
    if (m_server.isListening()) {
        m_server.close();
    }

    // Remove any stale socket file
    QLocalServer::removeServer(serverName);

    if (!m_server.listen(serverName)) {
        m_lastError = m_server.errorString();
        qWarning() << "IpcServer: Failed to listen on" << serverName
                   << "-" << m_lastError;
        return false;
    }

    qDebug() << "IpcServer: Listening on" << serverName;
    return true;
}

void IpcServer::close()
{
    // Disconnect all clients
    for (IpcChannel* client : qAsConst(m_clients)) {
        client->disconnect();
        client->deleteLater();
    }
    m_clients.clear();

    m_server.close();
}

bool IpcServer::isListening() const
{
    return m_server.isListening();
}

QString IpcServer::serverName() const
{
    return m_server.serverName();
}

int IpcServer::broadcast(const IpcMessage& message)
{
    int sentCount = 0;
    for (IpcChannel* client : qAsConst(m_clients)) {
        if (client->isConnected() && client->send(message)) {
            ++sentCount;
        }
    }
    return sentCount;
}

void IpcServer::onNewConnection()
{
    while (m_server.hasPendingConnections()) {
        QLocalSocket* socket = m_server.nextPendingConnection();
        if (!socket) continue;

        auto* channel = new IpcChannel(socket, this);

        connect(channel, &IpcChannel::stateChanged,
                this, &IpcServer::onChannelStateChanged);
        connect(channel, &IpcChannel::messageReceived,
                this, &IpcServer::onChannelMessageReceived);

        m_clients.append(channel);
        emit clientConnected(channel);

        qDebug() << "IpcServer: Client connected, total clients:" << m_clients.size();
    }
}

void IpcServer::onChannelStateChanged(ChannelState state)
{
    auto* channel = qobject_cast<IpcChannel*>(sender());
    if (!channel) return;

    if (state == ChannelState::Disconnected || state == ChannelState::Error) {
        removeClient(channel);
    }
}

void IpcServer::onChannelMessageReceived(const IpcMessage& message)
{
    auto* channel = qobject_cast<IpcChannel*>(sender());
    if (channel) {
        emit messageReceived(channel, message);
    }
}

void IpcServer::removeClient(IpcChannel* channel)
{
    if (m_clients.removeOne(channel)) {
        emit clientDisconnected(channel);
        channel->deleteLater();
        qDebug() << "IpcServer: Client disconnected, remaining:" << m_clients.size();
    }
}

} // namespace ipc
} // namespace automotive
