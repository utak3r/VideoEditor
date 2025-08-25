cmake_minimum_required(VERSION 3.30)

project(VideoEditor VERSION 0.1 LANGUAGES CXX)

set (PROJECT_NAME "VideoEditor")
set (PROJECT_VERSION_MAJOR 1)
set (PROJECT_VERSION_MINOR 3)
set (PROJECT_VERSION_PATCH 1)
set (PROJECT_COPYRIGHT "Piotr 'utak3r' Borys")

find_package(Git)
if(GIT_FOUND)
# Get the current working branch
execute_process(
  COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE PROJECT_GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND ${GIT_EXECUTABLE} log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE PROJECT_GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the number of commits since the commit_counter tag
# (which references the first commit in master branch)
execute_process(
  COMMAND ${GIT_EXECUTABLE} describe --match commit_counter
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE DESCRIBE_BUILD_COUNTER
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
string(REGEX MATCH "[0-9]+" PROJECT_BUILD_NUMBER ${DESCRIBE_BUILD_COUNTER})
endif()

# Get the current date and time
execute_process(
        COMMAND "cmd" " /c date /t"
        OUTPUT_VARIABLE PROJECT_BUILD_DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
        COMMAND "cmd" " /c echo %TIME%"
        OUTPUT_VARIABLE TIME
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
string(REGEX REPLACE "[^0-9]*(..:..:..).*" "\\1"
        PROJECT_BUILD_TIME "${TIME}")

# Configure the version.h file
configure_file(
        version.h.in
        version.h
)

# Configure the version.rc file
configure_file(
        version.rc.in
        version.rc
)

find_package(Qt6 REQUIRED COMPONENTS Widgets LinguistTools Multimedia MultimediaWidgets)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/../distrib")

set(PROJECT_ADDITIONAL_SOURCES
    ${CMAKE_SOURCE_DIR}/src/VEMainWindow.cpp
    ${CMAKE_SOURCE_DIR}/src/VEMainWindow.h
    ${CMAKE_SOURCE_DIR}/src/VEMainWindow.ui
    ${CMAKE_SOURCE_DIR}/src/TimelineMarks.cpp
    ${CMAKE_SOURCE_DIR}/src/TimelineMarks.h
    ${CMAKE_SOURCE_DIR}/src/VideoPreset.cpp
    ${CMAKE_SOURCE_DIR}/src/VideoPreset.h
    ${CMAKE_SOURCE_DIR}/src/VideoPlayer.cpp
    ${CMAKE_SOURCE_DIR}/src/VideoPlayer.h
    ${CMAKE_SOURCE_DIR}/src/VideoSlider.cpp
    ${CMAKE_SOURCE_DIR}/src/VideoSlider.h
    ${CMAKE_SOURCE_DIR}/src/CropRectangle.cpp
    ${CMAKE_SOURCE_DIR}/src/CropRectangle.h
    ${CMAKE_SOURCE_DIR}/src/Tools.cpp
    ${CMAKE_SOURCE_DIR}/src/Tools.h
    ${CMAKE_SOURCE_DIR}/src/ffmpegWrapper.h
    ${CMAKE_SOURCE_DIR}/src/ffmpeg.cpp
    ${CMAKE_SOURCE_DIR}/src/ffmpeg.h
    ${CMAKE_SOURCE_DIR}/src/Settings.cpp
    ${CMAKE_SOURCE_DIR}/src/Settings.h
    ${CMAKE_SOURCE_DIR}/src/ProcessRunnerDialog.cpp
    ${CMAKE_SOURCE_DIR}/src/ProcessRunnerDialog.h
    ${CMAKE_SOURCE_DIR}/src/ProcessRunnerDialog.ui
    ${CMAKE_SOURCE_DIR}/src/SettingsDialog.cpp
    ${CMAKE_SOURCE_DIR}/src/SettingsDialog.h
    ${CMAKE_SOURCE_DIR}/src/SettingsDialog.ui
    ${CMAKE_SOURCE_DIR}/src/VideoTranscoder.cpp
    ${CMAKE_SOURCE_DIR}/src/VideoTranscoder.h
    ${CMAKE_SOURCE_DIR}/src/EncodingPreset.cpp
    ${CMAKE_SOURCE_DIR}/src/EncodingPreset.h
    ${CMAKE_SOURCE_DIR}/src/EncodingPreset.ui
    ${CMAKE_SOURCE_DIR}/src/Codec.h
    ${CMAKE_SOURCE_DIR}/src/codec_x264.cpp
    ${CMAKE_SOURCE_DIR}/src/codec_x264.h
    ${CMAKE_SOURCE_DIR}/src/codec_x265.cpp
    ${CMAKE_SOURCE_DIR}/src/codec_x265.h
    ${CMAKE_SOURCE_DIR}/src/codec_aac.cpp
    ${CMAKE_SOURCE_DIR}/src/codec_aac.h
    ${CMAKE_SOURCE_DIR}/src/codec_dnxhd.cpp
    ${CMAKE_SOURCE_DIR}/src/codec_dnxhd.h
    ${CMAKE_SOURCE_DIR}/src/codec_pcms24le.cpp
    ${CMAKE_SOURCE_DIR}/src/codec_pcms24le.h
)

add_subdirectory(src)
add_subdirectory(test)

