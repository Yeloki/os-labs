cmake_minimum_required(VERSION 3.20)
project(lab-3)

set(CMAKE_C_STANDARD 11)

include_directories(../common)

set(SRC_EXTRA ../common/sub.h ../common/sub.c)

set(MAIN_EXEC src/main.c ${SRC_EXTRA})
add_executable(${PROJECT_NAME}-main ${MAIN_EXEC})

set(WORKER_EXEC src/worker.c ${SRC_EXTRA})
add_executable(${PROJECT_NAME}-worker ${WORKER_EXEC})

