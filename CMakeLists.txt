cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_COMPILER g++)

project(ChineseChess)

set(CMAKE_VERBOSE_MAKEFILE 1)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

# console version.
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
file(GLOB SRC_FILES "src/*.cpp")
add_executable(ChineseChess ${SRC_FILES})
target_link_libraries(ChineseChess)
#console version end.

# gui version.
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# include_directories(/3rd-party/SFML-2.6.1-windows-gcc-13.1.0-mingw-64-bit/SFML-2.6.1/include)
# link_directories(/3rd-party/SFML-2.6.1-windows-gcc-13.1.0-mingw-64-bit/SFML-2.6.1/lib)

# file(GLOB SRC_FILES "src/*.cpp")
# add_executable(ChineseChess ${SRC_FILES})
# target_link_libraries(ChineseChess sfml-graphics sfml-window sfml-system)
# gui version end.
