#ifndef PAGEMANAGER_H
#define PAGEMANAGER_H

#include <QObject>
#include <QTimer>
#include <iostream>

class PageManager : public QObject {
    Q_OBJECT
public:
    explicit PageManager(QObject *parent = nullptr) : QObject(parent) {}

    Q_INVOKABLE void showPage(const QString &pageName) {
        std::cout << "[PageManager] Changing page to: " << pageName.toStdString() << std::endl;
        emit pageChanged(pageName);
    }

signals:
    void pageChanged(const QString &pageName);
};

#endif // PAGEMANAGER_H
