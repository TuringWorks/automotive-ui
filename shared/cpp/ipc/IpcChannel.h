// IpcChannel.h
// IPC channel abstraction
// Part of: Shared Platform Layer

#ifndef AUTOMOTIVE_IPC_CHANNEL_H
#define AUTOMOTIVE_IPC_CHANNEL_H

#include "ipc/IpcMessage.h"
#include <QObject>
#include <QLocalSocket>
#include <QByteArray>
#include <memory>

namespace automotive {
namespace ipc {

/**
 * @brief IPC channel state
 */
enum class ChannelState {
    Disconnected,
    Connecting,
    Connected,
    Error
};

/**
 * @brief IPC channel for bidirectional message communication
 *
 * Wraps a QLocalSocket for local IPC between Driver UI and Infotainment UI.
 * Security: Validates all incoming messages (CR-INF-001)
 */
class IpcChannel : public QObject {
    Q_OBJECT
    Q_PROPERTY(ChannelState state READ state NOTIFY stateChanged)

public:
    explicit IpcChannel(QObject* parent = nullptr);
    explicit IpcChannel(QLocalSocket* socket, QObject* parent = nullptr);
    ~IpcChannel() override;

    /**
     * @brief Get current channel state
     */
    ChannelState state() const { return m_state; }

    /**
     * @brief Check if channel is connected
     */
    bool isConnected() const { return m_state == ChannelState::Connected; }

    /**
     * @brief Get last error message
     */
    QString lastError() const { return m_lastError; }

    /**
     * @brief Send a message over the channel
     * @param message Message to send
     * @return true if message was queued for sending
     */
    bool send(const IpcMessage& message);

    /**
     * @brief Connect to a named server
     * @param serverName Server name
     * @param timeoutMs Connection timeout in milliseconds
     * @return true if connection initiated (async)
     */
    bool connectToServer(const QString& serverName, int timeoutMs = 5000);

    /**
     * @brief Disconnect from server
     */
    void disconnect();

signals:
    /**
     * @brief Emitted when channel state changes
     */
    void stateChanged(ChannelState state);

    /**
     * @brief Emitted when a valid message is received
     */
    void messageReceived(const IpcMessage& message);

    /**
     * @brief Emitted when a malformed message is received
     * @param error Error description
     *
     * Security: CR-INF-001 - Malformed messages are logged but not processed
     */
    void malformedMessageReceived(const QString& error);

    /**
     * @brief Emitted on connection error
     */
    void errorOccurred(const QString& error);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QLocalSocket::LocalSocketError error);

private:
    void setState(ChannelState state);
    void processBuffer();
    bool parseMessage(const QByteArray& data);

    QLocalSocket* m_socket{nullptr};
    bool m_ownsSocket{false};
    ChannelState m_state{ChannelState::Disconnected};
    QString m_lastError;
    QByteArray m_readBuffer;
};

} // namespace ipc
} // namespace automotive

#endif // AUTOMOTIVE_IPC_CHANNEL_H
