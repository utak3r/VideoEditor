cmake_minimum_required(VERSION 3.30)

project(VEUnitTests LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-DGTEST_LANGUAGE_CXX17)

find_package(Threads REQUIRED)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# Don't install GoogleTest
option(INSTALL_GTEST "Enable installation of googletest." OFF)
FetchContent_MakeAvailable(googletest)

set(FFMPEG_DIR "C:/tools/ffmpeg")

include_directories(
	PUBLIC
    ${gtest_SOURCE_DIR}/include
    ${gmock_SOURCE_DIR}/include
    ..
    PRIVATE 
        "${FFMPEG_DIR}/include"
    )

add_executable(VEUnitTests
    main.cpp
    ffmpegMock.cpp
    test_VideoTranscoder.cpp
    )

target_link_libraries(VEUnitTests
    PUBLIC
    VideoEditor_lib
    "${FFMPEG_DIR}/lib/avformat.lib"
    "${FFMPEG_DIR}/lib/avcodec.lib"
    "${FFMPEG_DIR}/lib/avutil.lib"
    "${FFMPEG_DIR}/lib/swscale.lib"
    "${FFMPEG_DIR}/lib/swresample.lib"
    "${FFMPEG_DIR}/lib/avfilter.lib"
    PRIVATE
    Threads::Threads GTest::gtest_main
    Qt6::Widgets Qt6::Multimedia Qt6::MultimediaWidgets
    )

include(GoogleTest)
# deferring tests discovering, due to issues with finding Qt runtimes at this stage
#set(CMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE PRE_TEST)
gtest_discover_tests(VEUnitTests
    EXTRA_ARGS -v
    DISCOVERY_MODE PRE_TEST
    )
