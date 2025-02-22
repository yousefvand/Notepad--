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
find_package(
    Qt6
    REQUIRED
    COMPONENTS
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
    src/decoding/interpret_as_dialog.ui
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
    src/view/openinnewwindow.cpp
    src/view/openinnewwindow.h
    src/view/wordwrap.cpp
    src/view/wordwrap.h
    src/view/toggletoformertab.cpp
    src/view/toggletoformertab.h
    src/decoding/interpret_as_utf_8.cpp
    src/decoding/interpret_as_utf_8.h
    src/decoding/interpret_as_utf_8_without_bom.cpp
    src/decoding/interpret_as_utf_8_without_bom.h
    src/decoding/interpret_as_utf_16_be.cpp
    src/decoding/interpret_as_utf_16_be.h
    src/decoding/interpret_as_utf_16_le.cpp
    src/decoding/interpret_as_utf_16_le.h
    src/decoding/interpret_as_dialog.cpp
    src/decoding/interpret_as_dialog.h
    src/decoding/interpret_as_utf_8.cpp
    src/decoding/interpret_as_utf_8.h
    src/decoding/interpret_as_utf_7.cpp
    src/decoding/interpret_as_utf_7.h
    src/decoding/utf7handler.cpp
    src/decoding/utf7handler.h
    src/decoding/interpret_as_utf_32_le.cpp
    src/decoding/interpret_as_utf_32_le.h
    src/decoding/interpret_as_utf_32_be.cpp
    src/decoding/interpret_as_utf_32_be.h
    src/decoding/interpret_as_utf_32.cpp
    src/decoding/interpret_as_utf_32.h
    src/decoding/interpret_as_utf_16.cpp
    src/decoding/interpret_as_utf_16.h
    src/decoding/interpret_as_us_ascii.cpp
    src/decoding/interpret_as_us_ascii.h
    src/decoding/interpret_as_tscii.cpp
    src/decoding/interpret_as_tscii.h
    src/decoding/interpret_as_tis_620.cpp
    src/decoding/interpret_as_tis_620.h
    src/decoding/interpret_as_shift_jts.cpp
    src/decoding/interpret_as_shift_jts.h
    src/decoding/interpret_as_scsu.cpp
    src/decoding/interpret_as_scsu.h
    src/decoding/interpret_as_koi8_u.cpp
    src/decoding/interpret_as_koi8_u.h
    src/decoding/interpret_as_koi8_r.cpp
    src/decoding/interpret_as_koi8_r.h
    src/decoding/interpret_as_iso_8859_15.cpp
    src/decoding/interpret_as_iso_8859_15.h
    src/decoding/interpret_as_iso_8859_14.cpp
    src/decoding/interpret_as_iso_8859_14.h
    src/decoding/interpret_as_iso_8859_13.cpp
    src/decoding/interpret_as_iso_8859_13.h
    src/decoding/interpret_as_iso_8859_10.cpp
    src/decoding/interpret_as_iso_8859_10.h
    src/decoding/interpret_as_iso_8859_9.cpp
    src/decoding/interpret_as_iso_8859_9.h
    src/decoding/interpret_as_iso_8859_8.cpp
    src/decoding/interpret_as_iso_8859_8.h
    src/decoding/interpret_as_iso_8859_7.cpp
    src/decoding/interpret_as_iso_8859_7.h
    src/decoding/interpret_as_iso_8859_6.cpp
    src/decoding/interpret_as_iso_8859_6.h
    src/decoding/interpret_as_iso_8859_5.cpp
    src/decoding/interpret_as_iso_8859_5.h
    src/decoding/interpret_as_iso_8859_4.cpp
    src/decoding/interpret_as_iso_8859_4.h
    src/decoding/interpret_as_iso_8859_3.cpp
    src/decoding/interpret_as_iso_8859_3.h
    src/decoding/interpret_as_iso_8859_2.cpp
    src/decoding/interpret_as_iso_8859_2.h
    src/decoding/interpret_as_iso_8859_1.cpp
    src/decoding/interpret_as_iso_8859_1.h
    src/decoding/interpret_as_iso_2022_kr.cpp
    src/decoding/interpret_as_iso_2022_kr.h
    src/decoding/interpret_as_iso_2022_jp_2.cpp
    src/decoding/interpret_as_iso_2022_jp_2.h
    src/decoding/interpret_as_iso_2022_jp_1.cpp
    src/decoding/interpret_as_iso_2022_jp_1.h
    src/decoding/interpret_as_iso_2022_jp.cpp
    src/decoding/interpret_as_iso_2022_jp.h
    src/decoding/interpret_as_iso_2022_cn_ext.cpp
    src/decoding/interpret_as_iso_2022_cn_ext.h
    src/decoding/interpret_as_iso_2022_cn.cpp
    src/decoding/interpret_as_iso_2022_cn.h
    src/decoding/interpret_as_ibm_918.cpp
    src/decoding/interpret_as_ibm_918.h
    src/decoding/interpret_as_ibm_871.cpp
    src/decoding/interpret_as_ibm_871.h
    src/decoding/interpret_as_ibm_870.cpp
    src/decoding/interpret_as_ibm_870.h
    src/decoding/interpret_as_ibm_869.cpp
    src/decoding/interpret_as_ibm_869.h
    src/decoding/interpret_as_ibm_868.cpp
    src/decoding/interpret_as_ibm_868.h
    src/decoding/interpret_as_ibm_866.cpp
    src/decoding/interpret_as_ibm_866.h
    src/decoding/interpret_as_ibm_865.cpp
    src/decoding/interpret_as_ibm_865.h
    src/decoding/interpret_as_ibm_864.cpp
    src/decoding/interpret_as_ibm_864.h
    src/decoding/interpret_as_ibm_863.cpp
    src/decoding/interpret_as_ibm_863.h
    src/decoding/interpret_as_ibm_862.cpp
    src/decoding/interpret_as_ibm_862.h
    src/decoding/interpret_as_ibm_861.cpp
    src/decoding/interpret_as_ibm_861.h
    src/decoding/interpret_as_ibm_860.cpp
    src/decoding/interpret_as_ibm_860.h
    src/decoding/interpret_as_ibm_857.cpp
    src/decoding/interpret_as_ibm_857.h
    src/decoding/interpret_as_ibm_855.cpp
    src/decoding/interpret_as_ibm_855.h
    src/decoding/interpret_as_ibm_852.cpp
    src/decoding/interpret_as_ibm_852.h
    src/decoding/interpret_as_ibm_850.cpp
    src/decoding/interpret_as_ibm_850.h
    src/decoding/interpret_as_ibm_775.cpp
    src/decoding/interpret_as_ibm_775.h
    src/decoding/interpret_as_ibm_500.cpp
    src/decoding/interpret_as_ibm_500.h
    src/decoding/interpret_as_ibm_437.cpp
    src/decoding/interpret_as_ibm_437.h
    src/decoding/interpret_as_ibm_424.cpp
    src/decoding/interpret_as_ibm_424.h
    src/decoding/interpret_as_ibm_420.cpp
    src/decoding/interpret_as_ibm_420.h
    src/decoding/interpret_as_ibm_297.cpp
    src/decoding/interpret_as_ibm_297.h
    src/decoding/interpret_as_ibm_290.cpp
    src/decoding/interpret_as_ibm_290.h
    src/decoding/interpret_as_ibm_285.cpp
    src/decoding/interpret_as_ibm_285.h
    src/decoding/interpret_as_ibm_284.cpp
    src/decoding/interpret_as_ibm_284.h
    src/decoding/interpret_as_ibm_280.cpp
    src/decoding/interpret_as_ibm_280.h
    src/decoding/interpret_as_ibm_278.cpp
    src/decoding/interpret_as_ibm_278.h
    src/decoding/interpret_as_ibm_277.cpp
    src/decoding/interpret_as_ibm_277.h
    src/decoding/interpret_as_ibm_273.cpp
    src/decoding/interpret_as_ibm_273.h
    src/decoding/interpret_as_ibm_1047.cpp
    src/decoding/interpret_as_ibm_1047.h
    src/decoding/interpret_as_ibm_1026.cpp
    src/decoding/interpret_as_ibm_1026.h
    src/decoding/interpret_as_ibm_037.cpp
    src/decoding/interpret_as_ibm_037.h
    src/decoding/interpret_as_ibm_01149.cpp
    src/decoding/interpret_as_ibm_01149.h
    src/decoding/interpret_as_ibm_01148.cpp
    src/decoding/interpret_as_ibm_01148.h
    src/decoding/interpret_as_ibm_01147.cpp
    src/decoding/interpret_as_ibm_01147.h
    src/decoding/interpret_as_ibm_01146.cpp
    src/decoding/interpret_as_ibm_01146.h
    src/decoding/interpret_as_ibm_01145.cpp
    src/decoding/interpret_as_ibm_01145.h
    src/decoding/interpret_as_ibm_01144.cpp
    src/decoding/interpret_as_ibm_01144.h
    src/decoding/interpret_as_ibm_01143.cpp
    src/decoding/interpret_as_ibm_01143.h
    src/decoding/interpret_as_ibm_01142.cpp
    src/decoding/interpret_as_ibm_01142.h
    src/decoding/interpret_as_ibm_01141.cpp
    src/decoding/interpret_as_ibm_01141.h
    src/decoding/interpret_as_ibm_01140.cpp
    src/decoding/interpret_as_ibm_01140.h
    src/decoding/interpret_as_ibm_00858.cpp
    src/decoding/interpret_as_ibm_00858.h
    src/decoding/interpret_as_ibm_thai.cpp
    src/decoding/interpret_as_ibm_thai.h
    src/decoding/interpret_as_hz_gb_2312.cpp
    src/decoding/interpret_as_hz_gb_2312.h
    src/decoding/interpret_as_gbk.cpp
    src/decoding/interpret_as_gbk.h
    src/decoding/interpret_as_gb18030.cpp
    src/decoding/interpret_as_gb18030.h
    src/decoding/interpret_as_euc_jp.cpp
    src/decoding/interpret_as_euc_jp.h
    src/decoding/interpret_as_cesu_8.cpp
    src/decoding/interpret_as_cesu_8.h
    src/decoding/interpret_as_big5_hkscs.cpp
    src/decoding/interpret_as_big5_hkscs.h
    src/decoding/interpret_as_big5.cpp
    src/decoding/interpret_as_big5.h
    src/decoding/interpret_as_bocu_1.cpp
    src/decoding/interpret_as_bocu_1.h
    src/decoding/interpret_as_adobe_standard_encoding.cpp
    src/decoding/interpret_as_adobe_standard_encoding.h
    src/decoding/interpret_as_cp851.cpp
    src/decoding/interpret_as_cp851.h
    src/decoding/interpret_as_hp_roman8.cpp
    src/decoding/interpret_as_hp_roman8.h
    src/decoding/interpret_as_macintosh.cpp
    src/decoding/interpret_as_macintosh.h
    src/decoding/interpret_as_windows_1250.cpp
    src/decoding/interpret_as_windows_1250.h
    src/decoding/interpret_as_windows_1251.cpp
    src/decoding/interpret_as_windows_1251.h
    src/decoding/interpret_as_windows_1252.cpp
    src/decoding/interpret_as_windows_1252.h
    src/decoding/interpret_as_windows_1253.cpp
    src/decoding/interpret_as_windows_1253.h
    src/decoding/interpret_as_windows_1254.cpp
    src/decoding/interpret_as_windows_1254.h
    src/decoding/interpret_as_windows_1255.cpp
    src/decoding/interpret_as_windows_1255.h
    src/decoding/interpret_as_windows_1256.cpp
    src/decoding/interpret_as_windows_1256.h
    src/decoding/interpret_as_windows_1257.cpp
    src/decoding/interpret_as_windows_1257.h
    src/decoding/interpret_as_windows_1258.cpp
    src/decoding/interpret_as_windows_1258.h
    src/decoding/interpret_as_windows_949.cpp
    src/decoding/interpret_as_windows_949.h
    src/encoding/convert_to_dialog.cpp
    src/encoding/convert_to_dialog.h
    src/encoding/convert_to_utf_8.cpp
    src/encoding/convert_to_utf_8.h
    ${PROJECT_UI}
)

# Add Qt6 executable
qt_add_executable(Notepad--
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    src/encoding/convert_to_utf_7.h src/encoding/convert_to_utf_7.cpp
)

# Link Qt6 libraries
target_link_libraries(${PROJECT_NAME}
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
