cmake_minimum_required(VERSION 3.80)

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
    ${CMAKE_SOURCE_DIR}/VideoEditor.qrc
    ${PROJECT_ADDITIONAL_SOURCES}
)

set(app_icon_resource_windows "${CMAKE_SOURCE_DIR}/VideoEditor.rc")

qt_add_executable(VideoEditor
    MANUAL_FINALIZATION
    main.cpp
    ${PROJECT_SOURCES}
    ${CMAKE_SOURCE_DIR}/src/ffmpeg.h
    ${app_icon_resource_windows}
)

if(Qt6LinguistTools_FOUND)
    qt6_add_translations(VideoEditor TS_FILES ${TS_FILES} QM_FILES_OUTPUT_VARIABLE QM_FILES)
endif()


target_link_libraries(VideoEditor
    PRIVATE 
        Qt6::Widgets 
        Qt6::Multimedia 
        Qt6::MultimediaWidgets
    PUBLIC
        "FFmpeg::avformat"
        "FFmpeg::avcodec"
        "FFmpeg::avutil"
        "FFmpeg::swscale"
        "FFmpeg::swresample"
        "FFmpeg::avfilter"
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

install(CODE "
    execute_process(COMMAND \"${WINDEPLOYQT_EXECUTABLE}\" 
        --no-opengl-sw 
        --no-translations 
        --dir \"\${CMAKE_INSTALL_PREFIX}\" 
        \"\${CMAKE_INSTALL_PREFIX}/VideoEditor.exe\")
")

set(FFMPEG_LIBS avformat avcodec avutil swscale swresample avfilter)
foreach(lib IN LISTS FFMPEG_LIBS)
    get_target_property(LIB_PATH FFmpeg::${lib} IMPORTED_LOCATION)
    
    if(LIB_PATH)
        get_filename_component(LIB_NAME "${LIB_PATH}" NAME)
        
        message(STATUS "FFmpeg installation configuration: ${LIB_NAME} from path ${LIB_PATH}")
        
        install(FILES "${LIB_PATH}" 
                DESTINATION "."
                RENAME "${LIB_NAME}")
    else()
        message(WARNING "Couldn't find the path IMPORTED_LOCATION for FFmpeg::${lib}!")
    endif()
endforeach()
