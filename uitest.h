#ifndef UITEST_H
#define UITEST_H

#include <QtTest/QtTest>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>
#include <QSignalSpy>

class QmlTest : public QObject
{
    Q_OBJECT

private slots:
    void testButtonClick();  // ✅ 함수 선언 (정의 X)

private:
    QQuickView view;
};

#endif // UITEST_H
