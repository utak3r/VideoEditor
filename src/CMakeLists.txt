cmake_minimum_required(VERSION 3.30)

project(VideoEditorEXE VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TS_FILES ${CMAKE_SOURCE_DIR}/translations/VideoEditor_pl.ts)

set(PROJECT_SOURCES
    ${CMAKE_BINARY_DIR}/version.rc
    ${CMAKE_BINARY_DIR}/version.h
    ffmpegWrapper.cpp
    ${CMAKE_SOURCE_DIR}/VideoEditor.qrc
    ${PROJECT_ADDITIONAL_SOURCES}
)

set(app_icon_resource_windows "${CMAKE_SOURCE_DIR}/VideoEditor.rc")

qt_add_executable(VideoEditor
    MANUAL_FINALIZATION
    main.cpp
    ${PROJECT_SOURCES}
    ${app_icon_resource_windows}
)

if(Qt6LinguistTools_FOUND)
    qt6_add_translations(VideoEditor TS_FILES ${TS_FILES} QM_FILES_OUTPUT_VARIABLE QM_FILES)
endif()


#set(FFMPEG_DIR "C:/tools/ffmpeg")
find_path(FFMPEG_DIR include/libavcodec/avcodec.h)

target_include_directories(VideoEditor 
    PRIVATE 
        "${FFMPEG_DIR}/include"
)

target_link_libraries(VideoEditor
    PRIVATE 
        Qt6::Widgets 
        Qt6::Multimedia 
        Qt6::MultimediaWidgets
    PUBLIC
        "${FFMPEG_DIR}/lib/avformat.lib"
        "${FFMPEG_DIR}/lib/avcodec.lib"
        "${FFMPEG_DIR}/lib/avutil.lib"
        "${FFMPEG_DIR}/lib/swscale.lib"
        "${FFMPEG_DIR}/lib/swresample.lib"
        "${FFMPEG_DIR}/lib/avfilter.lib"
)

set(cppcheck
  "c:/Users/piotr/devel/Cppcheck/cppcheck.exe"
  "--enable=warning"
  "--inconclusive"
  "--force"
  "--inline-suppr"
)

set_target_properties(VideoEditor PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER VideoEditor.utak3r.pl
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    #CXX_CPPCHECK "${cppcheck}"
)
qt_finalize_executable(VideoEditor)

install (TARGETS VideoEditor
    RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}"
    )

install(FILES ${QM_FILES}
    DESTINATION "${CMAKE_INSTALL_PREFIX}/translations")

if (EXISTS ${WINDEPLOYQT_EXECUTABLE})
	install (CODE "
		execute_process(COMMAND \"${WINDEPLOYQT_EXECUTABLE}\" --dir \"${CMAKE_INSTALL_PREFIX}\" \"${CMAKE_BINARY_DIR}/src/Release/VideoEditor.exe\")
	")
    execute_process(
      COMMAND "${WINDEPLOYQT_EXECUTABLE}" --dir "${CMAKE_INSTALL_PREFIX}" "${CMAKE_BINARY_DIR}/src/Release/VideoEditor.exe"
    )
else()
    set(QT_LIBS
        "${CMAKE_PREFIX_PATH}/bin/Qt6Core.dll"
        "${CMAKE_PREFIX_PATH}/bin/Qt6Gui.dll"
        "${CMAKE_PREFIX_PATH}/bin/Qt6Widgets.dll"
        "${CMAKE_PREFIX_PATH}/bin/Qt6Multimedia.dll"
        "${CMAKE_PREFIX_PATH}/bin/Qt6MultimediaWidgets.dll"
        "${CMAKE_PREFIX_PATH}/bin/Qt6Network.dll"
        "${CMAKE_PREFIX_PATH}/bin/Qt6Svg.dll"
        )
    install(FILES ${QT_LIBS} DESTINATION "${CMAKE_INSTALL_PREFIX}")
    install(FILES
        "${CMAKE_PREFIX_PATH}/plugins/platforms/qwindows.dll"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/platforms"
        )
#    install(FILES
#        "${CMAKE_PREFIX_PATH}/plugins/styles/qwindowsvistastyle.dll"
#        DESTINATION "${CMAKE_INSTALL_PREFIX}/styles"
#        )
    install(FILES
        "${CMAKE_PREFIX_PATH}/plugins/iconengines/qsvgicon.dll"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/iconengines"
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
        DESTINATION "${CMAKE_INSTALL_PREFIX}/imageformats"
        )
endif()

# There's a bug in deployqt tool in Qt 6.4.0, so we need to fix it:
if (${Qt6_VERSION_MAJOR}.${Qt6_VERSION_MINOR}.${Qt6_VERSION_PATCH} VERSION_EQUAL "6.4.0")
    install(FILES
        "${CMAKE_PREFIX_PATH}/plugins/multimedia/windowsmediaplugin.dll"
        "${CMAKE_PREFIX_PATH}/plugins/multimedia/ffmpegmediaplugin.dll"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/multimedia"
        )
endif()

install(FILES 
    "${FFMPEG_DIR}/bin/avformat-61.dll"
    "${FFMPEG_DIR}/bin/avcodec-61.dll"
    "${FFMPEG_DIR}/bin/avutil-59.dll"
    "${FFMPEG_DIR}/bin/swscale-8.dll"
    "${FFMPEG_DIR}/bin/swresample-5.dll"
    "${FFMPEG_DIR}/bin/avfilter-10.dll"
    DESTINATION "${CMAKE_INSTALL_PREFIX}"
    )
