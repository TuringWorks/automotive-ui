// main.cpp
// Driver UI (Cluster) entry point
// Safety: This is a safety-relevant application

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QDebug>

#include "ClusterApplication.h"
#include "ClusterViewModel.h"

#include "signal/SignalHub.h"
#include "signal/VehicleSignals.h"
#include "sched/DeterministicScheduler.h"
#include "sched/TimeSource.h"
#include "logging/Logger.h"
#include "logging/LogSink.h"

using namespace automotive;

int main(int argc, char *argv[])
{
    // Initialize time source (monotonic clock)
    sched::TimeSource::instance().start();

    // Initialize logging
    auto consoleSink = std::make_shared<logging::ConsoleSink>();
    logging::Logger::instance().addSink(consoleSink);
    logging::Logger::instance().setMinLevel(logging::LogLevel::Debug);

    AUTO_LOG_INFO(QStringLiteral("cluster"), QStringLiteral("Driver UI starting..."));

    // Qt application
    QGuiApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("Automotive"));
    app.setApplicationName(QStringLiteral("DriverUI"));
    app.setApplicationVersion(QStringLiteral("1.0.0"));

    // Set style
    QQuickStyle::setStyle(QStringLiteral("Basic"));

    // Create signal hub
    signal::SignalHub signalHub;
    signal::VehicleSignalFactory::registerClusterSignals(signalHub);

    // Create scheduler
    sched::DeterministicScheduler scheduler;

    // Create cluster application
    driver::ClusterApplication clusterApp(&signalHub, &scheduler);

    // Create view model for QML
    driver::ClusterViewModel viewModel(clusterApp.stateModel(),
                                        clusterApp.alertManager(),
                                        clusterApp.telltaleManager(),
                                        clusterApp.degradedController());

    // Register types with QML
    qmlRegisterUncreatableType<driver::ClusterViewModel>(
        "DriverUI", 1, 0, "ClusterViewModel",
        QStringLiteral("ClusterViewModel is provided by context"));

    // Note: Enums are registered via ClusterViewModel

    // Create QML engine
    QQmlApplicationEngine engine;

    // Set import path for design system
    engine.addImportPath(QStringLiteral("qrc:/"));
    engine.addImportPath(QCoreApplication::applicationDirPath() + QStringLiteral("/qml"));

    // Expose objects to QML
    engine.rootContext()->setContextProperty(QStringLiteral("clusterViewModel"), &viewModel);
    engine.rootContext()->setContextProperty(QStringLiteral("clusterApp"), &clusterApp);
    engine.rootContext()->setContextProperty(QStringLiteral("faultInjector"),
                                              clusterApp.faultInjector());

    // Load QML
    const QUrl url(QStringLiteral("qrc:/DriverUI/app/qml/Main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);

    if (engine.rootObjects().isEmpty()) {
        AUTO_LOG_ERROR(QStringLiteral("cluster"), QStringLiteral("Failed to load QML"));
        return -1;
    }

    // Start the application
    clusterApp.start();

    AUTO_LOG_INFO(QStringLiteral("cluster"), QStringLiteral("Driver UI started successfully"));

    int result = app.exec();

    // Cleanup
    clusterApp.stop();
    AUTO_LOG_INFO(QStringLiteral("cluster"), QStringLiteral("Driver UI stopped"));

    return result;
}
