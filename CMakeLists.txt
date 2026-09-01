set(ORI_NAME orion)

option(ORI_WITH_SVG "Include SVG widgets" OFF)
option(ORI_WITH_TESTS "Include testing framework and internal tests" OFF)
option(ORI_WITH_STYLESHEETS "Use style-sheets rather than palette for colorizing selected state of widgets" OFF)
option(ORI_WITH_ZIP_HELP "Use packed format of help files for HelpWindow" OFF)
option(ORI_WITH_MD4C_HELP "Use md4c library for convertation markdown documentation to html" OFF)
option(ORI_WITH_SPELLCHECK "Include spell checker module" OFF)

set(ORI_SOURCES
    core/OriArg.h
    core/OriFilter.h
    core/OriFloatingPoint.h
    core/OriResult.h
    core/OriTemplates.h
    core/OriVersion.h
    dialogs/OriBasicConfigDlg.h dialogs/OriBasicConfigDlg.cpp
    dialogs/OriConfigDlg.h dialogs/OriConfigDlg.cpp
    helpers/OriDialogs.h helpers/OriDialogs.cpp
    helpers/OriLayouts.h helpers/OriLayouts.cpp
    helpers/OriTheme.h helpers/OriTheme.cpp
    helpers/OriTools.h helpers/OriTools.cpp
    helpers/OriWidgets.h helpers/OriWidgets.cpp
    helpers/OriWindows.h helpers/OriWindows.cpp
    tools/OriDebug.h tools/OriDebug.cpp
    tools/OriHelpWindow.h tools/OriHelpWindow.cpp
    tools/OriHighlighter.h tools/OriHighlighter.cpp
    tools/OriLog.h tools/OriLog.cpp
    tools/OriLoremIpsum.h tools/OriLoremIpsum.cpp
    tools/OriMessageBus.h tools/OriMessageBus.cpp
    tools/OriMruList.h tools/OriMruList.cpp
    tools/OriPersistentState.cpp tools/OriPersistentState.h
    tools/OriPetname.h tools/OriPetname.cpp
    tools/OriSettings.h tools/OriSettings.cpp
    tools/OriStyler.h tools/OriStyler.cpp
    tools/OriTranslator.h tools/OriTranslator.cpp
    tools/OriUpdater.h tools/OriUpdater.cpp
    widgets/OriActions.h widgets/OriActions.cpp
    widgets/OriBackWidget.h widgets/OriBackWidget.cpp
    widgets/OriCharMap.h widgets/OriCharMap.cpp
    widgets/OriCodeEditor.h widgets/OriCodeEditor.cpp
    widgets/OriColorSelectors.h widgets/OriColorSelectors.cpp
    widgets/OriFlatToolBar.h
    widgets/OriFlowLayout.h widgets/OriFlowLayout.cpp
    widgets/OriInfoPanel.h widgets/OriInfoPanel.cpp
    widgets/OriLabels.h widgets/OriLabels.cpp
    widgets/OriLangsMenu.h widgets/OriLangsMenu.cpp
    widgets/OriMdiToolBar.h widgets/OriMdiToolBar.cpp
    widgets/OriMenuToolButton.h widgets/OriMenuToolButton.cpp
    widgets/OriMruMenu.h widgets/OriMruMenu.cpp
    widgets/OriOptionsGroup.h widgets/OriOptionsGroup.cpp
    widgets/OriPopupButton.h widgets/OriPopupButton.cpp
    widgets/OriPopupMessage.h widgets/OriPopupMessage.cpp
    widgets/OriSelectableTile.h widgets/OriSelectableTile.cpp
    widgets/OriStatusBar.h widgets/OriStatusBar.cpp
    widgets/OriStylesMenu.h widgets/OriStylesMenu.cpp
    widgets/OriTableWidgetBase.h widgets/OriTableWidgetBase.cpp
    widgets/OriValueEdit.h widgets/OriValueEdit.cpp
)

if(ORI_WITH_TESTS)
    list(APPEND ORI_SOURCES
        testing/OriTestBase.h testing/OriTestBase.cpp
        testing/OriTestManager.h
        testing/OriTestWindow.h testing/OriTestWindow.cpp
        testing/OriTestWindowTests.cpp
        testing/OriTimeMeter.h testing/OriTimeMeter.cpp
        tests/ori_test_Filter.cpp
        tests/ori_test_Math.cpp
        tests/ori_test_Templates.cpp
        tests/ori_test_Version.cpp
        tests/orion_tests.h
    )
endif()

if(ORI_WITH_SVG)
    list(APPEND ORI_SOURCES
        widgets/OriSvgView.h widgets/OriSvgView.cpp
    )
endif()

if(ORI_WITH_SPELLCHECK)
    list(APPEND ORI_SOURCES
        tools/OriSpellcheck.h tools/OriSpellcheck.cpp
        tools/OriSpellcheckEngine.h tools/OriSpellcheckEngine.cpp
    )

    # Let a parent provide Hunspell while keeping Orion usable on its own.
    if(NOT TARGET Hunspell::Hunspell)
        list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
        find_package(Hunspell REQUIRED)
    endif()
endif()

# https://doc.qt.io/qt-6.5/cmake-qt5-and-qt6-compatibility.html
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Network Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Network Widgets)

add_library(${ORI_NAME} STATIC
    ${ORI_SOURCES}
)
add_library(Ori::orion ALIAS ${ORI_NAME})

# Orion owns its Qt code-generation and language requirements.
set_target_properties(${ORI_NAME} PROPERTIES
    AUTOMOC ON
    CXX_EXTENSIONS OFF
)
target_compile_features(${ORI_NAME}
    PUBLIC cxx_std_17
    PRIVATE cxx_std_20
)

target_link_libraries(${ORI_NAME}
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Widgets
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Network
)

if(ORI_WITH_SPELLCHECK)
    target_link_libraries(${ORI_NAME} PRIVATE Hunspell::Hunspell)
endif()

if(ORI_WITH_SVG)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Svg)
    target_link_libraries(${ORI_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Svg)
endif()

target_compile_definitions(${ORI_NAME} PRIVATE
    QT_USE_QSTRINGBUILDER
)

if(ORI_WITH_STYLESHEETS)
    # This definition controls declarations in the PUBLIC OriTheme header.
    target_compile_definitions(${ORI_NAME} PUBLIC ORI_USE_STYLE_SHEETS)
endif()

if(ORI_WITH_ZIP_HELP)
    find_package(libzip CONFIG REQUIRED)
    target_link_libraries(${ORI_NAME} PRIVATE libzip::zip)
    target_compile_definitions(${ORI_NAME} PRIVATE ORI_USE_ZIP_HELP)
endif()

if(ORI_WITH_MD4C_HELP)
    find_package(md4c CONFIG REQUIRED)
    target_link_libraries(${ORI_NAME} PRIVATE md4c::md4c-html)
    target_compile_definitions(${ORI_NAME} PRIVATE ORI_USE_MD4C_HELP)
endif()

target_include_directories(${ORI_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Static-library resources are compiled into each final executable to retain registration.
set(qrc_files "${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc")
if(ORI_WITH_TESTS)
    list(APPEND qrc_files "${CMAKE_CURRENT_SOURCE_DIR}/testing/test.qrc")
endif()
set_property(GLOBAL PROPERTY ORI_QRC_FILES "${qrc_files}")
function(orion_add_resources target)
    if(NOT TARGET "${target}")
        message(FATAL_ERROR "Unknown Orion resource target: ${target}")
    endif()
    get_property(ori_qrc_files GLOBAL PROPERTY ORI_QRC_FILES)
    target_sources("${target}" PRIVATE ${ori_qrc_files})
    set_property(TARGET "${target}" PROPERTY AUTORCC ON)
endfunction()
