// IpcClient.h
// IPC client for connecting to servers
// Part of: Shared Platform Layer

#ifndef AUTOMOTIVE_IPC_CLIENT_H
#define AUTOMOTIVE_IPC_CLIENT_H

#include "ipc/IpcChannel.h"
#include <QObject>
#include <QTimer>

namespace automotive {
namespace ipc {

/**
 * @brief IPC client with automatic reconnection
 *
 * Provides reliable connection to an IPC server with:
 * - Automatic reconnection on disconnect
 * - Heartbeat monitoring
 * - Connection state management
 */
class IpcClient : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)

public:
    explicit IpcClient(QObject* parent = nullptr);
    ~IpcClient() override;

    /**
     * @brief Connect to a server
     * @param serverName Server name to connect to
     */
    void connectToServer(const QString& serverName);

    /**
     * @brief Disconnect from server
     */
    void disconnect();

    /**
     * @brief Check if connected
     */
    bool isConnected() const;

    /**
     * @brief Send a message
     * @param message Message to send
     * @return true if message was sent
     */
    bool send(const IpcMessage& message);

    /**
     * @brief Set reconnection interval
     * @param intervalMs Interval in milliseconds (0 to disable)
     */
    void setReconnectInterval(int intervalMs);

    /**
     * @brief Set heartbeat interval
     * @param intervalMs Interval in milliseconds (0 to disable)
     */
    void setHeartbeatInterval(int intervalMs);

signals:
    /**
     * @brief Emitted when connection state changes
     */
    void connectedChanged(bool connected);

    /**
     * @brief Emitted when a message is received
     */
    void messageReceived(const IpcMessage& message);

    /**
     * @brief Emitted on connection error
     */
    void errorOccurred(const QString& error);

private slots:
    void onChannelStateChanged(ChannelState state);
    void onReconnectTimer();
    void onHeartbeatTimer();

private:
    IpcChannel m_channel;
    QString m_serverName;
    QTimer m_reconnectTimer;
    QTimer m_heartbeatTimer;
    int m_reconnectIntervalMs{5000};
    int m_heartbeatIntervalMs{1000};
    bool m_shouldConnect{false};
};

} // namespace ipc
} // namespace automotive

#endif // AUTOMOTIVE_IPC_CLIENT_H
