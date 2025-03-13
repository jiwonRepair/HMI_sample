#ifndef UITEST_H
#define UITEST_H

#include <QTest> //#include <QtTest/QtTest> or #include <QtTest>는 개별 모듈 지정이므로 경고 발생 함
#include <QQmlApplicationEngine>
#include <QSignalSpy> // ✅ QSignalSpy 추가 (신호 감지 용도)

class QmlTest : public QObject
{
    Q_OBJECT

private:
    QQmlApplicationEngine engine;
    QObject *rootObject = nullptr;
    QObject *pageManager = nullptr;

    void setUp(); // ✅ QML 및 객체 초기화 함수

private slots:
    void testPageNavigation();  // ✅ 페이지 전환 테스트
    void testPageNavigationWithButton(); // ✅ 페이지 버튼 클릭 테스트
    void testFullPageNavigationWithButtons();
    void clickButtonAndVerify(const QString &buttonId, QSignalSpy &pageChangedSpy);
    void testStressFullPageNavigation1(); //선언된 함수마다 10000ms이상 쓸수없음
    void testStressFullPageNavigation2();
    void testStressFullPageNavigation3();
    void testStressFullPageNavigation4();
    void testStressFullPageNavigation5();
    void testStressFullPageNavigation6();
    void runStressTest(int start, int end); //
};

#endif // UITEST_H
