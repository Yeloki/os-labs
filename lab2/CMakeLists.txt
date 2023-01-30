cmake_minimum_required(VERSION 3.20)
project(lab-2)

set(CMAKE_C_STANDARD 11)

include_directories(../common)

set(SRC_EXTRA ../common/sub.h ../common/sub.c)

set(MASTER_EXEC src/main.c ${SRC_EXTRA})
add_executable(${PROJECT_NAME}-master ${MASTER_EXEC})

set(SLAVE_EXEC src/slave.c ${SRC_EXTRA})
add_executable(${PROJECT_NAME}-slave ${SLAVE_EXEC})

