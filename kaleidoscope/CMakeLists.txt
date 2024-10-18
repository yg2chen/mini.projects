cmake_minimum_required(VERSION 3.20)
project(llvm_kaleidoscope)

set(CMAKE_CXX_STANDARD 17)
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM : ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in : ${LLVM_DIR}")
message(STATUS "LLVM include_directories : ${LLVM_INCLUDE_DIRS}")
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_subdirectory(src)

# Find the libraries that correspond to the LLVM components that we wish to use
llvm_map_components_to_libnames(llvm_libs core native orcjit)
# Link against LLVM libraries
target_link_libraries(kaleidoscope PUBLIC ${llvm_libs})
