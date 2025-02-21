cmake_minimum_required(VERSION 3.10)

project(Chinese_Chess_With_AI)

option(USING_CPP "using C++ version, turn off this to compile C version." ON)

if (USING_CPP)
	message("-- using C++ version.")

	set(CMAKE_CXX_STANDARD 11)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
	
	add_executable(${PROJECT_NAME} "Chinese_Chess_With_AI.cpp")
else()
	message("-- using C version.")
	
	set(CMAKE_C_STANDARD 99)
	set(CMAKE_C_STANDARD_REQUIRED ON)
	set(CMAKE_C_EXTENSIONS OFF)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")

	add_executable(${PROJECT_NAME} "Chinese_Chess_With_AI.c")
endif()
