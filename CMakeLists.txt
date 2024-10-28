cmake_minimum_required(VERSION 3.16)

project(Notepad-- VERSION 0.1 LANGUAGES CXX)

# Enable automatic handling of Qt6 features
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Use C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable strict compiler warnings (optional)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find Qt6 packages in a single call
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Concurrent
    PrintSupport
    LinguistTools
)

# Translation files (optional)
set(TS_FILES translations/Notepad--_en_GB.ts)

# Define project sources
set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.ui
    src/mainwindow.cpp
    src/mainwindow.h
    src/document.cpp
    src/document.h
    src/codeeditor.cpp
    src/codeeditor.h
    src/fileloaderworker.cpp
    src/fileloaderworker.h
    src/languages/languagemanager.cpp
    src/languages/languagemanager.h
    src/languages/cppsyntaxhighlighter.cpp
    src/languages/cppsyntaxhighlighter.h
    src/languages/pythonsyntaxhighlighter.cpp
    src/languages/pythonsyntaxhighlighter.h
    src/mainwindow/fileoperations.cpp
    src/mainwindow/fileoperations.h
    src/mainwindow/recentfiles.cpp
    src/mainwindow/recentfiles.h
    src/mainwindow/settings.cpp
    src/mainwindow/settings.h
    src/mainwindow/helpers.cpp
    src/mainwindow/helpers.h
    src/mainwindow/formatting.cpp
    src/mainwindow/formatting.h
    src/mainwindow/session.cpp
    src/mainwindow/session.h
)

# Add Qt6 executable
qt_add_executable(Notepad--
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

# Link Qt6 libraries
target_link_libraries(Notepad--
    PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Concurrent Qt6::PrintSupport
)

# Set properties for macOS/iOS and Windows
set_target_properties(Notepad-- PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    WIN32_EXECUTABLE TRUE
)

# Installation setup
include(GNUInstallDirs)
install(TARGETS Notepad--
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Optional: Add translation files to target (if available)
qt_add_translation(QM_FILES ${TS_FILES})
if(QM_FILES)
    install(FILES ${QM_FILES} DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/translations)
endif()

# Finalize the Qt6 executable setup
qt_finalize_executable(Notepad--)
