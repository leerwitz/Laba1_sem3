cmake_minimum_required(VERSION 3.23)
project(htable)

set(CMAKE_CXX_STANDARD 20)

add_executable(htable main.cpp Hash_table.hpp LinkedList.hpp VectorSorter.hpp Tests.hpp cache.hpp story.hpp)
