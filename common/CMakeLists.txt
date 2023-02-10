cmake_minimum_required(VERSION 3.20)
project(test-common)

set(CMAKE_C_STANDARD 11)

include_directories(../common)

set(SRC sub.h sub.c)

set(TEST_EXEC tests.c ${SRC})
add_executable(${PROJECT_NAME}-sub ${TEST_EXEC})
