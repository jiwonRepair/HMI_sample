#ifdef _WIN32
#include <winsock2.h>  // 반드시 windows.h 보다 먼저!
#endif

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "cpumonitor.h"
#include "pagemanager.h"
#include "BatteryMonitor.h"
#include "memorymonitor.h"
#include "diskmonitor.h"
#include "hardwarestatusmodel.h"
#include "wifimonitor.h"
#include "loggingdecorator.h"
#include "appguard.h"
#include "osfileManager.h"
#include "wifioptimizer.h"

//#include "main.moc"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    PageManager pageManager;
    CpuMonitor cpuMonitor;
    BatteryMonitor batteryMonitor;
    MemoryMonitor memoryMonitor;  // ✅ 객체 생성
    DiskMonitor diskMonitor;
    HardwareStatusModel hardwareStatusModel;
    WifiMonitor wifiMonitor;
    OsFileManager osFileManager;
    WifiOptimizer wifiOptimizer;

    LoggingDecorator logger("HMI Application");

    // ✅ 예외 처리를 감싸서 QML 컨텍스트에 객체 등록
    logger.execute([&]() {
        engine.rootContext()->setContextProperty("wifiOptimizer", &wifiOptimizer);
        engine.rootContext()->setContextProperty("osFileManager", &osFileManager);
        engine.rootContext()->setContextProperty("pageManager", &pageManager);
        engine.rootContext()->setContextProperty("cpuMonitor", &cpuMonitor);
        engine.rootContext()->setContextProperty("batteryMonitor", &batteryMonitor);
        engine.rootContext()->setContextProperty("memoryMonitor", &memoryMonitor);
        engine.rootContext()->setContextProperty("diskMonitor", &diskMonitor);
        engine.rootContext()->setContextProperty("hardwareStatusModel", &hardwareStatusModel);
        engine.rootContext()->setContextProperty("wifiMonitor", &wifiMonitor);
    });

    // ✅ 예외 발생 시 안전한 QML 로딩
    logger.execute([&]() {
        //engine.load(QUrl::fromLocalFile("D:/qtprojects/HMI_sample/Main.qml"));
        QString qmlPath = QCoreApplication::applicationDirPath() + "/Main.qml";
        engine.load(QUrl::fromLocalFile(qmlPath));
    });

    if (engine.rootObjects().isEmpty())
        return -1;

    // ✅ RAII 패턴 적용 (AppGuard 사용)
    AppGuard appGuard(app, engine);
    return appGuard.run();

    //return app.exec();
}

