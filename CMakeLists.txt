cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_COMPILER g++)

project(ChineseChess)

set(CMAKE_VERBOSE_MAKEFILE 1)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

# -mwindows is for gui version, if you want console version, never add -mwindows, like:
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

include_directories(/mingw64/include)
link_directories(/mingw64/lib)

file(GLOB SRC_FILES "src/*.cpp")
add_executable(ChineseChess ${SRC_FILES})

# sfml libs don't required if you want console version, like:
# target_link_libraries(ChineseChess)
target_link_libraries(ChineseChess sfml-graphics sfml-window sfml-system)
