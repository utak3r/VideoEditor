cmake_minimum_required(VERSION 3.5)

project(VideoEditor VERSION 0.1 LANGUAGES CXX)

set (PROJECT_NAME "VideoEditor")
set (PROJECT_VERSION_MAJOR 1)
set (PROJECT_VERSION_MINOR 1)
set (PROJECT_VERSION_PATCH 0)
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



set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
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
