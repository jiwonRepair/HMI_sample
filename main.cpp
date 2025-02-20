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

    engine.rootContext()->setContextProperty("pageManager", &pageManager);
    engine.rootContext()->setContextProperty("cpuMonitor", &cpuMonitor);
    engine.rootContext()->setContextProperty("batteryMonitor", &batteryMonitor);
    engine.rootContext()->setContextProperty("memoryMonitor", &memoryMonitor);
    engine.rootContext()->setContextProperty("diskMonitor", &diskMonitor);        
    engine.rootContext()->setContextProperty("hardwareStatusModel", &hardwareStatusModel);
    engine.rootContext()->setContextProperty("wifiMonitor", &wifiMonitor);

    engine.load(QUrl::fromLocalFile("D:/qtprojects/HMI_sample/Main.qml"));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
