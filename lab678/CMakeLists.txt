cmake_minimum_required(VERSION 3.20)
project(lab-678)

set(CMAKE_CXX_STANDARD 23)

set(SRC_EXTRA)

set(CORE_EXEC src/main.cpp ${SRC_EXTRA})
add_executable(${PROJECT_NAME}-core ${CORE_EXEC})

set(NODE_EXEC src/node.cpp ${SRC_EXTRA})
add_executable(${PROJECT_NAME}-worker ${NODE_EXEC})

target_link_libraries(${PROJECT_NAME}-core zmq)
target_link_libraries(${PROJECT_NAME}-worker zmq)