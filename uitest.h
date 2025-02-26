#ifndef UITEST_H
#define UITEST_H

#include <QtTest/QtTest>
#include <QQmlApplicationEngine>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>

class QmlTest : public QObject
{
    Q_OBJECT

private:
    QQmlApplicationEngine engine;
    QObject *rootObject = nullptr;
    QObject *pageManager = nullptr;

    void setUp(); // ✅ QML 및 객체 초기화 함수

private slots:
    //void testButtonClick();     // ✅ 버튼 클릭 테스트
    void testPageNavigation();  // ✅ 페이지 전환 테스트
    void testPageNavigationWithButton();
    void testFullPageNavigationWithButtons();
    void clickButtonAndVerify(const QString &buttonId, QSignalSpy &pageChangedSpy);
    void testStressFullPageNavigation();
};

#endif // UITEST_H
