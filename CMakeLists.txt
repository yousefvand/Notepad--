cmake_minimum_required(VERSION 3.16)

project(Notepad-- VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Use C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 libraries
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Concurrent)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets PrintSupport)

# Translation file path in 'translations' folder
set(TS_FILES translations/Notepad--_en_GB.ts)

# Project sources, organized under 'src' and 'src/languages' folders
set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    src/document.cpp
    src/document.h
    src/codeeditor.cpp
    src/codeeditor.h
    src/languages/cppsyntaxhighlighter.cpp
    src/languages/cppsyntaxhighlighter.h
    src/languages/pythonsyntaxhighlighter.cpp
    src/languages/pythonsyntaxhighlighter.h
    src/languages/languagemanager.cpp
    src/languages/languagemanager.h
)

# Add executable for Qt6
qt_add_executable(Notepad--
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    src/fileloaderworker.h src/fileloaderworker.cpp

)

# Link necessary Qt6 libraries
target_link_libraries(Notepad--
    PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Concurrent Qt6::PrintSupport
)

# Set target properties for macOS/iOS
set_target_properties(Notepad-- PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Installation setup
include(GNUInstallDirs)
install(TARGETS Notepad--
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Finalize executable for Qt6
qt_finalize_executable(Notepad--)
