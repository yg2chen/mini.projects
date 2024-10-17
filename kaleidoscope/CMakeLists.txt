cmake_minimum_required(VERSION 3.20)
project(llvm_kaleidoscope)

set(CMAKE_CXX_STANDARD 17)
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM : ${LLVM_PACKAGE_VERSION}")
# message(STATUS "Using LLVMConfig.cmake in : ${LLVM_DIR}")
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})


add_subdirectory(src)
# add_subdirectory(tests)

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader mcjit native)

## Link against LLVM libraries
target_link_libraries(kaleidoscope PUBLIC ${llvm_libs})
