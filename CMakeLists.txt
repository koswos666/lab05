cmake_minimum_required(VERSION 3.14)
project(banking)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Автоматическая загрузка GTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

add_library(banking STATIC
    include/banking/Account.h
    include/banking/Transaction.h
    src/Account.cpp
    src/Transaction.cpp
)

target_include_directories(banking PUBLIC include)

enable_testing()

add_executable(banking_tests
    test/main.cpp
    test/AccountTest.cpp
    test/TransactionTest.cpp
)

target_link_libraries(banking_tests
    banking
    gtest_main
    gmock_main
)

include(GoogleTest)
gtest_discover_tests(banking_tests)
