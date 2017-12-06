cmake_minimum_required(VERSION 2.8)

project(PERSISTENT_SET)

include_directories(${PERSISTENT_SET_SOURCE_DIR})

add_executable(perset 	my_pointers/my_shared_ptr.h
			my_pointers/my_linked_ptr.h
			persistent_set.h
			test.cpp)


if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -O2 -pedantic")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
endif()
