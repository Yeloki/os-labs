cmake_minimum_required(VERSION 3.20)
project(lab-3)

set(CMAKE_CXX_STANDARD 23)

include_directories(../common/cpp/)

set(SRC_EXTRA ../common/cpp/ArgsParser.cpp ../common/cpp/ArgsParser.hpp ../common/cpp/threads.hpp)

set(MAIN_EXEC src/main.cpp ${SRC_EXTRA})
add_executable(${PROJECT_NAME}-main ${MAIN_EXEC})

