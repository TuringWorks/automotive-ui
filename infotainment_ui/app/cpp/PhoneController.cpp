// PhoneController.cpp
// Phone controller implementation

#include "PhoneController.h"

namespace automotive {
namespace infotainment {

PhoneController::PhoneController(QObject* parent)
    : QObject(parent)
{
    loadMockData();
}

PhoneController::~PhoneController() = default;

void PhoneController::dial(const QString& number)
{
    m_callerNumber = number;
    m_callerName = QStringLiteral("Dialing...");
    m_inCall = true;
    m_callDuration = 0;
    emit callStateChanged();
}

void PhoneController::hangUp()
{
    m_inCall = false;
    m_callerName.clear();
    m_callerNumber.clear();
    m_callDuration = 0;
    emit callStateChanged();
}

void PhoneController::answer()
{
    // Mock: already answered
    emit callStateChanged();
}

void PhoneController::reject()
{
    hangUp();
}

void PhoneController::mute(bool muted)
{
    m_muted = muted;
}

void PhoneController::loadMockData()
{
    m_contacts.clear();

    auto addContact = [this](const QString& name, const QString& number) {
        QVariantMap contact;
        contact[QStringLiteral("name")] = name;
        contact[QStringLiteral("number")] = number;
        contact[QStringLiteral("favorite")] = false;
        m_contacts.append(contact);
    };

    addContact(QStringLiteral("Alice"), QStringLiteral("+1 555-0101"));
    addContact(QStringLiteral("Bob"), QStringLiteral("+1 555-0102"));
    addContact(QStringLiteral("Carol"), QStringLiteral("+1 555-0103"));
    addContact(QStringLiteral("David"), QStringLiteral("+1 555-0104"));

    m_recentCalls.clear();

    auto addRecent = [this](const QString& name, const QString& number,
                           const QString& type, const QString& time) {
        QVariantMap call;
        call[QStringLiteral("name")] = name;
        call[QStringLiteral("number")] = number;
        call[QStringLiteral("type")] = type;  // incoming, outgoing, missed
        call[QStringLiteral("time")] = time;
        m_recentCalls.append(call);
    };

    addRecent(QStringLiteral("Alice"), QStringLiteral("+1 555-0101"),
              QStringLiteral("incoming"), QStringLiteral("2 hours ago"));
    addRecent(QStringLiteral("Bob"), QStringLiteral("+1 555-0102"),
              QStringLiteral("missed"), QStringLiteral("Yesterday"));

    emit contactsChanged();
    emit recentCallsChanged();
}

} // namespace infotainment
} // namespace automotive
