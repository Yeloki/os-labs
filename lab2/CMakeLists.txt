project(lab-1)

set(CMAKE_C_STANDARD 11)

set(MASTER_EXEC src/main.c src/sub.h)
add_executable(${PROJECT_NAME}-master ${MASTER_EXEC})

set(SLAVE_EXEC src/slave.c src/sub.h)
add_executable(${PROJECT_NAME}-slave ${SLAVE_EXEC})
