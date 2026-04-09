set(FFMPEG_COMPONENTS avcodec avformat avutil swscale swresample avfilter)

if(NOT FFMPEG_ROOT)
    message(WARNING "FFMPEG_ROOT is not set. Searching might fail due to NO_DEFAULT_PATH.")
endif()

foreach(component ${FFMPEG_COMPONENTS})
    # Headers
    find_path(FFmpeg_${component}_INCLUDE_DIR
        NAMES lib${component}/${component}.h
        PATHS ${FFMPEG_ROOT}
        PATH_SUFFIXES include
        NO_DEFAULT_PATH
    )

    # Import libs
    find_library(FFmpeg_${component}_LIBRARY
        NAMES ${component}
        PATHS ${FFMPEG_ROOT}
        PATH_SUFFIXES lib
        NO_DEFAULT_PATH
    )

    # Shared libs
    find_file(FFmpeg_${component}_DLL
        NAMES 
            ${component}-61.dll ${component}-60.dll ${component}-59.dll 
            ${component}-10.dll ${component}-8.dll ${component}-5.dll
        PATHS ${FFMPEG_ROOT}
        PATH_SUFFIXES bin
        NO_DEFAULT_PATH
    )

    if(FFmpeg_${component}_LIBRARY AND FFmpeg_${component}_INCLUDE_DIR)
        set(FFmpeg_${component}_FOUND TRUE)
        if(NOT TARGET FFmpeg::${component})
            add_library(FFmpeg::${component} SHARED IMPORTED)
            set_target_properties(FFmpeg::${component} PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${FFmpeg_${component}_INCLUDE_DIR}"
                IMPORTED_IMPLIB "${FFmpeg_${component}_LIBRARY}"
                IMPORTED_LOCATION "${FFmpeg_${component}_DLL}"
            )
        endif()
    endif()
endforeach()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FFmpeg
    REQUIRED_VARS FFmpeg_avcodec_LIBRARY FFmpeg_avcodec_INCLUDE_DIR
    HANDLE_COMPONENTS
)
