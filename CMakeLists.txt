cmake_minimum_required(VERSION 3.16)

project(test4 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ✅ Qt 라이브러리 찾기 (Gui, Qml, Quick 추가)
find_package(Qt6 REQUIRED COMPONENTS Gui Qml Quick Test)

qt_standard_project_setup(REQUIRES 6.5)

# ✅ QML 파일 리스트
set(QML_FILES
    Main.qml
    Page2.qml
    Page3.qml
    Page4.qml
    Page5.qml
    CpuGauge.qml
    BatteryGauge.qml
    MemoryGauge.qml
    DiskGauge.qml
    WifiGauge.qml
    HWstatus.qml
    graph.qml
)

# ✅ 애플리케이션 실행 파일 추가
qt_add_executable(apptest4
    main.cpp
    pagemanager.h pagemanager.cpp
    cpumonitor.h cpumonitor.cpp
    batterymonitor.h batterymonitor.cpp
    memorymonitor.h memorymonitor.cpp
    diskmonitor.h diskmonitor.cpp
    hardwarestatusmodel.h hardwarestatusmodel.cpp
    wifimonitor.h wifimonitor.cpp
)

# ✅ QML 모듈 추가
qt_add_qml_module(apptest4
    URI test4
    VERSION 1.0
    QML_FILES ${QML_FILES}
    QML_FILES
    SOURCES loggingdecorator.h loggingdecorator.cpp
    SOURCES appguard.h appguard.cpp
)

# ✅ Qt 라이브러리 링크 추가
target_link_libraries(apptest4 PRIVATE Qt6::Gui Qt6::Qml Qt6::Quick)

# ✅ Windows 시스템 라이브러리 추가 (🚀 오류 해결)
if(WIN32)
    target_link_libraries(apptest4 PRIVATE setupapi wlanapi)
endif()

# ✅ 테스트 실행 파일 추가 (필수 .cpp 파일 포함)
add_executable(tst_qmltest
    tst_qmltest.cpp
    uitest.cpp
    pagemanager.h pagemanager.cpp
    cpumonitor.h cpumonitor.cpp
    batterymonitor.h batterymonitor.cpp
    memorymonitor.h memorymonitor.cpp
    diskmonitor.h diskmonitor.cpp
    hardwarestatusmodel.h hardwarestatusmodel.cpp
    wifimonitor.h wifimonitor.cpp
    uitest.h
)

# ✅ 테스트 실행 파일에 Qt 라이브러리 추가
target_link_libraries(tst_qmltest PRIVATE Qt6::Gui Qt6::Qml Qt6::Quick Qt6::Test)

# ✅ Windows 시스템 라이브러리 추가 (🚀 오류 해결)
if(WIN32)
    target_link_libraries(tst_qmltest PRIVATE setupapi wlanapi)
endif()

include(GNUInstallDirs)

install(TARGETS apptest4
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


# # ✅ CTest 사용 가능하도록 설정
# enable_testing()

# # ✅ 테스트 등록
# add_test(NAME QmlTest COMMAND tst_qmltest)

# # ✅ 테스트 실행 시 환경 변수 적용 (QTEST_FUNCTION_TIMEOUT=0)
# set_tests_properties(QmlTest PROPERTIES
#     ENVIRONMENT "QTEST_FUNCTION_TIMEOUT=0"
# )
