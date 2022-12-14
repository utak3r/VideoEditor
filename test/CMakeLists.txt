cmake_minimum_required(VERSION 3.5)

project(VEUnitTests LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-DGTEST_LANGUAGE_CXX11)

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

include_directories(
    ${gtest_SOURCE_DIR}/include
    ${gmock_SOURCE_DIR}/include
    ..
    )

find_package(Qt6 REQUIRED COMPONENTS Widgets LinguistTools Multimedia MultimediaWidgets)

add_executable(VEUnitTests
    main.cpp
    tst_videoeditor.cpp
    )
target_link_libraries(VEUnitTests
    PUBLIC
    VideoEditor_lib
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
