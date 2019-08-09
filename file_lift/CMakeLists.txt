cmake_minimum_required(VERSION 3.1)

find_package(LLVM REQUIRED CONFIG)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

include_directories(${CMAKE_SOURCE_DIR}/../io_spec/proj) #dir containing header file of pass to be linked
link_directories(${CMAKE_SOURCE_DIR}/../io_spec/build/proj) #dir containing library/.so file of pass to be linked

add_subdirectory(proj)  #project dir containing CMakeLists.txt file