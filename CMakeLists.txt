cmake_minimum_required(VERSION 3.16)

project(test4 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.5)

set(QML_FILES
    Main.qml
    Page1.qml
    Page2.qml
    Page3.qml
    Page4.qml
    Page5.qml
    CpuGauge.qml
)

qt_add_executable(apptest4
    main.cpp
)

qt_add_qml_module(apptest4
    URI test4
    VERSION 1.0
    QML_FILES
        Main.qml
        QML_FILES CpuGauge.qml
        QML_FILES
        QML_FILES Page2.qml
        QML_FILES Page3.qml
        QML_FILES Page4.qml
        QML_FILES Page5.qml
        SOURCES pagemanager.h pagemanager.cpp
        SOURCES cpumonitor.h cpumonitor.cpp
        SOURCES batterymonitor.h batterymonitor.cpp
        QML_FILES BatteryGauge.qml
        SOURCES memorymonitor.h memorymonitor.cpp
        QML_FILES MemoryGauge.qml
        SOURCES diskmonitor.h diskmonitor.cpp
        QML_FILES DiskGauge.qml
        SOURCES hardwarestatusmodel.h hardwarestatusmodel.cpp
        QML_FILES HWstatus.qml
        SOURCES wifimonitor.h wifimonitor.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(apptest4 PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.apptest4
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

find_package(Qt6 REQUIRED COMPONENTS Quick Charts)


target_link_libraries(
    apptest4
    PRIVATE Qt6::Quick
    setupapi
    Bthprops
    Wlanapi
    #Qt6::Charts
    #Qt6::qtchartsqml2  # ✅ Qt6Charts 추가
)

include(GNUInstallDirs)
install(TARGETS apptest4
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)



