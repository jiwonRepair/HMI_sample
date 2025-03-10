#include "LoggingDecorator.h"
#include <iostream>

LoggingDecorator::LoggingDecorator(const QString &name) : componentName(name) {}

void LoggingDecorator::execute(std::function<void()> func) {
    try {
        func();
    } catch (const std::exception &e) {
        std::cerr << "[ERROR] " << componentName.toStdString() << " Exception: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[ERROR] " << componentName.toStdString() << " Unknown exception occurred!" << std::endl;
    }
}
