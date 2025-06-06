cmake_minimum_required(VERSION 3.14)
project(lab05)

set(CMAKE_CXX_STANDARD 17)

option(ENABLE_COVERAGE "Enable coverage reporting" ON)
if(ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.13.0  
)
FetchContent_MakeAvailable(googletest)

add_subdirectory(banking)

add_executable(tests
    test/test.cpp
    test/tests2.cpp  # Добавляем второй тестовый файл
)

target_link_libraries(tests PRIVATE
    banking
    gtest_main
    gmock_main
)

enable_testing()
include(GoogleTest)
gtest_discover_tests(tests)
