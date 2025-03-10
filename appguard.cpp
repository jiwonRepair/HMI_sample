#include "appguard.h"
#include <iostream>

AppGuard::AppGuard(QGuiApplication &app, QQmlApplicationEngine &engine)
    : appInstance(app), engineInstance(engine) {}

int AppGuard::run() {
    try {
        return appInstance.exec();
    } catch (const std::exception &e) {
        std::cerr << "[CRITICAL ERROR] Application Exception: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "[CRITICAL ERROR] Unknown fatal error occurred!" << std::endl;
        return -1;
    }
}
