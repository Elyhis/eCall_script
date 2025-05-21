set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

list(APPEND CMAKE_PREFIX_PATH "C:/Qt/6.9.0/msvc2022_64/lib/cmake/Qt6")

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets SerialPort)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets SerialPort)


qt_add_executable(app
    MANUAL_FINALIZATION
    main.cpp
    nmea.cpp
    scenario.cpp
    mathFormulaJO.cpp
    mainwindow.cpp
    mainwindow.ui
    ../inc/mainwindow.h
)

target_include_directories(app PRIVATE ${PROJECT_SOURCE_DIR}/inc/sdx_api ${PROJECT_SOURCE_DIR}/inc)

# Lier avec la bibliothèque sdx_api si nécessaire
target_link_libraries(app PRIVATE sdx_api Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::SerialPort)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
    BUNDLE_ID_OPTION ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

qt_finalize_executable(app)