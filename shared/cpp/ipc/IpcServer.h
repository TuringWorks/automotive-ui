// IpcServer.h
// IPC server for accepting client connections
// Part of: Shared Platform Layer

#ifndef AUTOMOTIVE_IPC_SERVER_H
#define AUTOMOTIVE_IPC_SERVER_H

#include "ipc/IpcChannel.h"
#include <QObject>
#include <QLocalServer>
#include <QVector>
#include <memory>

namespace automotive {
namespace ipc {

/**
 * @brief IPC server for accepting client connections
 *
 * Manages multiple client channels for broadcast and targeted messaging.
 */
class IpcServer : public QObject {
    Q_OBJECT

public:
    static constexpr const char* DRIVER_UI_SERVER = "automotive_driver_ui";
    static constexpr const char* INFOTAINMENT_SERVER = "automotive_infotainment";

    explicit IpcServer(QObject* parent = nullptr);
    ~IpcServer() override;

    /**
     * @brief Start listening on the specified server name
     * @param serverName Name for the local server
     * @return true if server started successfully
     */
    bool listen(const QString& serverName);

    /**
     * @brief Stop the server
     */
    void close();

    /**
     * @brief Check if server is listening
     */
    bool isListening() const;

    /**
     * @brief Get server name
     */
    QString serverName() const;

    /**
     * @brief Get number of connected clients
     */
    int clientCount() const { return m_clients.size(); }

    /**
     * @brief Broadcast a message to all connected clients
     * @param message Message to broadcast
     * @return Number of clients message was sent to
     */
    int broadcast(const IpcMessage& message);

    /**
     * @brief Get last error message
     */
    QString lastError() const { return m_lastError; }

signals:
    /**
     * @brief Emitted when a new client connects
     */
    void clientConnected(IpcChannel* channel);

    /**
     * @brief Emitted when a client disconnects
     */
    void clientDisconnected(IpcChannel* channel);

    /**
     * @brief Emitted when a message is received from any client
     */
    void messageReceived(IpcChannel* channel, const IpcMessage& message);

private slots:
    void onNewConnection();
    void onChannelStateChanged(ChannelState state);
    void onChannelMessageReceived(const IpcMessage& message);

private:
    void removeClient(IpcChannel* channel);

    QLocalServer m_server;
    QVector<IpcChannel*> m_clients;
    QString m_lastError;
};

} // namespace ipc
} // namespace automotive

#endif // AUTOMOTIVE_IPC_SERVER_H
