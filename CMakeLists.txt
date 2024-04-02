cmake_minimum_required(VERSION 3.5)
project(weather VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)


add_executable(${PROJECT_NAME} main.cpp)


find_package(cpr CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE cpr::cpr)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

set(CMAKE_EXE_LINKER_FLAGS "--static")

include_directories(${PDCURSES_INCLUDE_DIR})

target_compile_options(${PROJECT_NAME}  PRIVATE "-I{PDCURSES_INCLUDE_DIR}")

target_link_libraries(${PROJECT_NAME}  PRIVATE "${PDCURSES_LIB_DIR}/pdcurses.lib")


link_directories(lib)
add_subdirectory(lib)
