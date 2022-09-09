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

set(PROJECT_SOURCES
        main.cpp
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
        ${TS_FILES}
)

qt_add_executable(VideoEditor
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
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
