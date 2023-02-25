cmake_minimum_required(VERSION 3.20)
project(lab-678)

set(CMAKE_CXX_STANDARD 23)

set(SRC_EXTRA src/zmq.hpp src/zmq_addon.hpp)

set(CORE_EXEC src/core.cpp ${SRC_EXTRA})
add_executable(${PROJECT_NAME}-core ${CORE_EXEC})

set(NODE_EXEC src/node.cpp ${SRC_EXTRA})
add_executable(${PROJECT_NAME}-worker ${NODE_EXEC})

