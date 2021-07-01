cmake_minimum_required(VERSION 3.8)

project(K_ADAPTABILITY)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(CPLEX REQUIRED)

set(SOURCE_FILES
        ./src/uncertainty.cpp
        ./src/problemInfo.cpp
        ./src/indexingTools.cpp
        ./src/robustSolver.cpp
    )

add_library(kadaptability ${SOURCE_FILES})
target_include_directories(kadaptability PUBLIC include)

target_link_libraries(kadaptability PUBLIC solver::cplex)

add_subdirectory(examples/knp)
add_subdirectory(examples/psp)
add_subdirectory(examples/spp)
add_subdirectory(examples/mmkp_continuous)
add_subdirectory(examples/mmkp_discrete)
