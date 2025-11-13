cmake_minimum_required(VERSION 3.15)

project(CClubIncome VERSION 1.0
                    DESCRIPTION "A project that parses computer club log file and counts this club's income"
                    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER g++)

find_package(GTest REQUIRED)

add_library(cclublib STATIC src/CClubParser.cpp
                            src/CClubHandler.cpp)
target_include_directories(cclublib PUBLIC include)

add_executable(cclub src/main.cpp)
target_link_libraries(cclub PRIVATE cclublib)

if(GTest_FOUND)
    enable_testing()

    add_executable(cclub_tests
        tests/test_parser.cpp
        tests/test_handler.cpp
    )
    target_link_libraries(cclub_tests PRIVATE cclublib GTest::GTest GTest::Main)
    add_test(NAME CClubTests COMMAND cclub_tests)
else()
    message(WARNING "GoogleTest not found, tests will not be built.")
endif()