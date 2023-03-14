cmake_minimum_required(VERSION 3.20)
project(lab-678)

set(CMAKE_CXX_STANDARD 23)

set(SRC_EXTRA)

# load in pkg-config support
find_package(PkgConfig)
## use pkg-config to get hints for 0mq locations
pkg_check_modules(PC_ZeroMQ QUIET zmq)

## use the hint from above to find where 'zmq.hpp' is located
find_path(ZeroMQ_INCLUDE_DIR
        NAMES zmq.hpp
        PATHS ${PC_ZeroMQ_INCLUDE_DIRS}
        )

## use the hint from above to find the location of libzmq
find_library(ZeroMQ_LIBRARY
        NAMES zmq
        PATHS ${PC_ZeroMQ_LIBRARY_DIRS}
        )





set(CORE_EXEC src/core.cpp ${SRC_EXTRA})
add_executable(${PROJECT_NAME}-core ${CORE_EXEC})

set(NODE_EXEC src/node.cpp ${SRC_EXTRA})
add_executable(${PROJECT_NAME}-worker ${NODE_EXEC})

## add the include directory to our compile directives
include_directories(${PROJECT_NAME} PUBLIC ${ZeroMQ_INCLUDE_DIR})
## at the 0mq library to our link directive
link_libraries(${PROJECT_NAME} PUBLIC ${ZeroMQ_LIBRARY})