cmake_minimum_required(VERSION 3.20)
project(lab-3)

set(CMAKE_CXX_STANDARD 23)


set(SRC_EXTRA ../common/cpp/ArgsParser.cpp ../common/cpp/ArgsParser.hpp)

set(MAIN_EXEC src/main.cpp ${SRC_EXTRA})
add_executable(${PROJECT_NAME}-main ${MAIN_EXEC})

set(WORKER_EXEC src/worker.cpp ${SRC_EXTRA})
add_executable(${PROJECT_NAME}-worker ${WORKER_EXEC})

