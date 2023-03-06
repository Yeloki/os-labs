cmake_minimum_required(VERSION 3.25)

project(lab-5)

set(CMAKE_CXX_STANDARD 23)

include_directories(../common/cpp/)
set(primes_src src/libs/primes.cpp src/libs/primes.hpp)
set(areas_src src/libs/areas.cpp src/libs/areas.hpp)
set(headers src/libs/primes.hpp src/libs/areas.hpp)
add_library(areas_st STATIC ${areas_src})
add_library(areas_sh SHARED ${areas_src})

add_library(primes_st STATIC ${primes_src})
add_library(primes_sh SHARED ${primes_src})


#link_libraries(areas_st primes_st)
set(SRC_EXTRA ${headers})
set(MAIN_EXEC src/main.cpp ${SRC_EXTRA})
add_executable(${PROJECT_NAME}-main ${MAIN_EXEC})


