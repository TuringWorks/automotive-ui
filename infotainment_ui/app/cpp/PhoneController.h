// PhoneController.h
// Phone/Bluetooth controller (mock)

#ifndef AUTOMOTIVE_PHONE_CONTROLLER_H
#define AUTOMOTIVE_PHONE_CONTROLLER_H

#include <QObject>
#include <QString>
#include <QVariantList>

namespace automotive {
namespace infotainment {

/**
 * @brief Phone controller (mock implementation)
 */
class PhoneController : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
    Q_PROPERTY(QString phoneName READ phoneName NOTIFY phoneNameChanged)
    Q_PROPERTY(bool inCall READ isInCall NOTIFY callStateChanged)
    Q_PROPERTY(QString callerName READ callerName NOTIFY callStateChanged)
    Q_PROPERTY(QString callerNumber READ callerNumber NOTIFY callStateChanged)
    Q_PROPERTY(int callDuration READ callDuration NOTIFY callDurationChanged)
    Q_PROPERTY(QVariantList contacts READ contacts NOTIFY contactsChanged)
    Q_PROPERTY(QVariantList recentCalls READ recentCalls NOTIFY recentCallsChanged)

public:
    explicit PhoneController(QObject* parent = nullptr);
    ~PhoneController() override;

    bool isConnected() const { return m_connected; }
    QString phoneName() const { return m_phoneName; }
    bool isInCall() const { return m_inCall; }
    QString callerName() const { return m_callerName; }
    QString callerNumber() const { return m_callerNumber; }
    int callDuration() const { return m_callDuration; }
    QVariantList contacts() const { return m_contacts; }
    QVariantList recentCalls() const { return m_recentCalls; }

public slots:
    void dial(const QString& number);
    void hangUp();
    void answer();
    void reject();
    void mute(bool muted);

signals:
    void connectedChanged(bool connected);
    void phoneNameChanged(const QString& name);
    void callStateChanged();
    void callDurationChanged(int duration);
    void contactsChanged();
    void recentCallsChanged();
    void incomingCall(const QString& name, const QString& number);

private:
    void loadMockData();

    bool m_connected{true};
    QString m_phoneName{QStringLiteral("My Phone")};
    bool m_inCall{false};
    QString m_callerName;
    QString m_callerNumber;
    int m_callDuration{0};
    bool m_muted{false};
    QVariantList m_contacts;
    QVariantList m_recentCalls;
};

} // namespace infotainment
} // namespace automotive

#endif // AUTOMOTIVE_PHONE_CONTROLLER_H
