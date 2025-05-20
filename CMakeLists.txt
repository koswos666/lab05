cmake_minimum_required(VERSION 3.14)
project(lab05)


cmake_policy(SET CMP0135 NEW)


option(ENABLE_COVERAGE "Enable coverage reporting" ON)
if(ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif()


include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

add_subdirectory(banking)

add_executable(tests
    test/test.cpp
)

target_link_libraries(tests PRIVATE
    banking
    GTest::GTest
    GMock::GMock
)

enable_testing()
include(GoogleTest)
gtest_discover_tests(tests)

find_package(GTest REQUIRED)
find_package(GMock REQUIRED)

target_link_libraries(tests PRIVATE
    GTest::GTest
    GMock::GMock
)
