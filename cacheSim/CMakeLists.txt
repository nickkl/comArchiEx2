cmake_minimum_required(VERSION 3.14)
project(cacheSim)

set(CMAKE_CXX_STANDARD 11)

add_executable(cacheSim cacheSim.cpp cache.cpp cache.h memUnit.cpp memUnit.h LRU.cpp LRU.h)