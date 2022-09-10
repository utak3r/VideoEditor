cmake_minimum_required(VERSION 3.5)

project(VideoEditor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets LinguistTools Multimedia MultimediaWidgets)

set(TS_FILES VideoEditor_pl_PL.ts)

set(PROJECT_ADDITIONAL_SOURCES
    VEMainWindow.cpp
    VEMainWindow.h
    VEMainWindow.ui
    TimelineMarks.cpp
    TimelineMarks.h
    VideoPreset.cpp
    VideoPreset.h
    VideoPlayer.cpp
    VideoPlayer.h
    VideoSlider.cpp
    VideoSlider.h
    ffmpeg.cpp
    ffmpeg.h
    Settings.cpp
    Settings.h
    ProcessRunnerDialog.cpp
    ProcessRunnerDialog.h
    ProcessRunnerDialog.ui
    SettingsDialog.cpp
    SettingsDialog.h
    SettingsDialog.ui
)
set(PROJECT_SOURCES
        main.cpp
        VideoEditor.qrc
        ${PROJECT_ADDITIONAL_SOURCES}
        ${TS_FILES}
)

set(app_icon_resource_windows "${CMAKE_SOURCE_DIR}/VideoEditor.rc")

qt_add_executable(VideoEditor
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${app_icon_resource_windows}
)
qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

target_link_libraries(VideoEditor PRIVATE Qt6::Widgets Qt6::Multimedia Qt6::MultimediaWidgets)

set_target_properties(VideoEditor PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER VideoEditor.utak3r.pl
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
qt_finalize_executable(VideoEditor)

# this library serves for linking against unit tests
qt_add_library(VideoEditor_lib STATIC ${PROJECT_ADDITIONAL_SOURCES})
target_link_libraries(VideoEditor_lib PRIVATE Qt6::Widgets Qt6::Multimedia Qt6::MultimediaWidgets)


add_subdirectory(test)

set(INSTALL_DESTDIR "${PROJECT_SOURCE_DIR}/../distrib")
install (TARGETS VideoEditor
    RUNTIME DESTINATION "${INSTALL_DESTDIR}"
    BUNDLE DESTINATION "${INSTALL_DESTDIR}"
    LIBRARY DESTINATION "${INSTALL_DESTDIR}"
    )
set(QT_LIBS
    "${CMAKE_PREFIX_PATH}/bin/Qt6Widgets.dll"
    "${CMAKE_PREFIX_PATH}/bin/Qt6Multimedia.dll"
    "${CMAKE_PREFIX_PATH}/bin/Qt6MultimediaWidgets.dll"
    "${CMAKE_PREFIX_PATH}/bin/Qt6Gui.dll"
    "${CMAKE_PREFIX_PATH}/bin/Qt6Core.dll"
    "${CMAKE_PREFIX_PATH}/bin/Qt6Network.dll"
    )
install(FILES ${QT_LIBS} DESTINATION "${INSTALL_DESTDIR}")
install(FILES
    "${CMAKE_PREFIX_PATH}/plugins/platforms/qwindows.dll"
    DESTINATION "${INSTALL_DESTDIR}/platforms"
    )
install(FILES
    "${CMAKE_PREFIX_PATH}/plugins/styles/qwindowsvistastyle.dll"
    DESTINATION "${INSTALL_DESTDIR}/styles"
    )
install(FILES
    "${CMAKE_PREFIX_PATH}/plugins/iconengines/qsvgicon.dll"
    DESTINATION "${INSTALL_DESTDIR}/iconengines"
    )
install(FILES
    "${CMAKE_PREFIX_PATH}/plugins/imageformats/qgif.dll"
    "${CMAKE_PREFIX_PATH}/plugins/imageformats/qicns.dll"
    "${CMAKE_PREFIX_PATH}/plugins/imageformats/qico.dll"
    "${CMAKE_PREFIX_PATH}/plugins/imageformats/qjpeg.dll"
    "${CMAKE_PREFIX_PATH}/plugins/imageformats/qpdf.dll"
    "${CMAKE_PREFIX_PATH}/plugins/imageformats/qsvg.dll"
    "${CMAKE_PREFIX_PATH}/plugins/imageformats/qtga.dll"
    "${CMAKE_PREFIX_PATH}/plugins/imageformats/qtiff.dll"
    "${CMAKE_PREFIX_PATH}/plugins/imageformats/qwbmp.dll"
    "${CMAKE_PREFIX_PATH}/plugins/imageformats/qwebp.dll"
    DESTINATION "${INSTALL_DESTDIR}/imageformats"
    )
