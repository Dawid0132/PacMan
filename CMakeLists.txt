cmake_minimum_required(VERSION 3.26)
project(PacMan)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED COMPONENTS SDL2)
find_package(SDL2_TTF REQUIRED COMPONENTS SDL2_ttf)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_ttf::SDL2_ttf)
