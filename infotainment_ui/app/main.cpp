// main.cpp
// Infotainment UI entry point
// Security: Cybersecurity-critical application

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QDebug>

#include "AppFramework.h"
#include "PermissionManager.h"
#include "SecureSettings.h"
#include "MediaController.h"
#include "NavigationController.h"
#include "PhoneController.h"
#include "ProjectionController.h"
#include "VehicleSettingsController.h"

#include "logging/Logger.h"
#include "logging/LogSink.h"
#include "sched/TimeSource.h"

using namespace automotive;

int main(int argc, char *argv[])
{
    // Initialize time source
    sched::TimeSource::instance().start();

    // Initialize logging with PII redaction (CR-INF-003)
    auto consoleSink = std::make_shared<logging::ConsoleSink>();
    logging::Logger::instance().addSink(consoleSink);
    logging::Logger::instance().setMinLevel(logging::LogLevel::Debug);
    logging::Logger::instance().setPiiRedactionEnabled(true);

    AUTO_LOG_INFO("infotainment", "Infotainment UI starting...");

    // Qt application
    QGuiApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("Automotive"));
    app.setApplicationName(QStringLiteral("InfotainmentUI"));
    app.setApplicationVersion(QStringLiteral("1.0.0"));

    // Set style
    QQuickStyle::setStyle(QStringLiteral("Basic"));

    // Create permission manager (CR-INF-002)
    infotainment::PermissionManager permissionManager;

    // Create secure settings (CR-INF-003)
    infotainment::SecureSettings secureSettings;

    // Create controllers
    infotainment::MediaController mediaController;
    infotainment::NavigationController navController;
    infotainment::PhoneController phoneController;
    infotainment::ProjectionController projectionController;
    infotainment::VehicleSettingsController vehicleController(&permissionManager);

    // Create app framework
    infotainment::AppFramework appFramework;
    appFramework.setPermissionManager(&permissionManager);
    appFramework.setSecureSettings(&secureSettings);

    // Register types with QML
    qmlRegisterUncreatableType<infotainment::PermissionManager>(
        "InfotainmentUI", 1, 0, "PermissionManager",
        QStringLiteral("PermissionManager is provided by context"));

    // Create QML engine
    QQmlApplicationEngine engine;

    // Set import path for design system
    engine.addImportPath(QStringLiteral("qrc:/"));
    engine.addImportPath(QCoreApplication::applicationDirPath() + QStringLiteral("/qml"));

    // Expose objects to QML
    engine.rootContext()->setContextProperty(QStringLiteral("appFramework"), &appFramework);
    engine.rootContext()->setContextProperty(QStringLiteral("permissionManager"), &permissionManager);
    engine.rootContext()->setContextProperty(QStringLiteral("secureSettings"), &secureSettings);
    engine.rootContext()->setContextProperty(QStringLiteral("mediaController"), &mediaController);
    engine.rootContext()->setContextProperty(QStringLiteral("navController"), &navController);
    engine.rootContext()->setContextProperty(QStringLiteral("phoneController"), &phoneController);
    engine.rootContext()->setContextProperty(QStringLiteral("projectionController"), &projectionController);
    engine.rootContext()->setContextProperty(QStringLiteral("vehicleController"), &vehicleController);

    // Load QML
    const QUrl url(QStringLiteral("qrc:/InfotainmentUI/app/qml/Main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);

    if (engine.rootObjects().isEmpty()) {
        AUTO_LOG_ERROR("infotainment", "Failed to load QML");
        return -1;
    }

    AUTO_LOG_INFO("infotainment", "Infotainment UI started successfully");

    return app.exec();
}
