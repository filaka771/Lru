cmake_minimum_required(VERSION 3.10)

project(LRU)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=fuzzer,address,undefined -g -O1")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=fuzzer,address,undefined")

include_directories(
    test/include
    lru_backend/include
)

add_executable(lru_fuzz test/src/lru_fuzz_test.cpp)
