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

set(PROJECT_UI
    src/mainwindow.ui
    src/find/finddialog.ui
    src/replace/replacedialog.ui
    src/indentation/indentationdialog.ui
    src/systemfind/systemfinddialog.ui
    src/systemreplace/systemreplacedialog.ui
    src/systemsearchresultdialog.ui
    src/aboutdialog.ui
)

set(CMAKE_AUTOUIC_SEARCH_PATHS src)

set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/document.cpp
    src/document.h
    src/codeeditor.cpp
    src/codeeditor.h
    src/fileloaderworker.cpp
    src/fileloaderworker.h
    src/helpers.cpp
    src/helpers.h
    src/settings.cpp
    src/settings.h
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
    src/mainwindow/formatting.cpp
    src/mainwindow/formatting.h
    src/mainwindow/session.cpp
    src/mainwindow/session.h
    src/mainwindow/textoperations.cpp
    src/mainwindow/textoperations.h
    src/indentation/indentationdialog.cpp
    src/indentation/indentationdialog.h
    src/indentation/indentationmanager.cpp
    src/indentation/indentationmanager.h
    src/replace/replacedialog.cpp
    src/replace/replacedialog.h
    src/replace/replace.cpp
    src/replace/replace.h
    src/search/search.cpp
    src/search/search.h
    src/search/searchoptions.h
    src/search/filesearchworker.cpp
    src/search/filesearchworker.h
    src/find/finddialog.cpp
    src/find/finddialog.h
    src/find/find.cpp
    src/find/find.h
    src/systemfind/systemfind.cpp
    src/systemfind/systemfind.h
    src/systemfind/systemfinddialog.cpp
    src/systemfind/systemfinddialog.h
    src/systemfind/richtextdelegate.cpp
    src/systemfind/richtextdelegate.h
    src/systemreplace/systemreplacedialog.cpp
    src/systemreplace/systemreplacedialog.h
    src/systemreplace/systemreplace.cpp
    src/systemreplace/systemreplace.h
    src/systemsearchresultdialog.cpp
    src/systemsearchresultdialog.h
    src/systemtextdelegate.cpp
    src/systemtextdelegate.h
    src/mainwindow/mainwindowconfigloader.cpp
    src/mainwindow/mainwindowconfigloader.h
    src/aboutdialog.cpp
    src/aboutdialog.h
    src/view/movetootherview.cpp
    src/view/movetootherview.h
    src/view/movetonewview.cpp
    src/view/movetonewview.h
    ${PROJECT_UI}
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
