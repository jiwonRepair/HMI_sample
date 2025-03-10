#ifndef APPGUARD_H
#define APPGUARD_H

#include <QGuiApplication>
#include <QQmlApplicationEngine>

class AppGuard {
public:
    AppGuard(QGuiApplication &app, QQmlApplicationEngine &engine);

    int run();

private:
    QGuiApplication &appInstance;
    QQmlApplicationEngine &engineInstance;
};

#endif // APPGUARD_H
