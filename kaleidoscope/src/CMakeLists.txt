include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/lexer
  ${CMAKE_CURRENT_SOURCE_DIR}/logger
  ${CMAKE_CURRENT_SOURCE_DIR}/ast
  ${CMAKE_CURRENT_SOURCE_DIR}/utils
  ${CMAKE_CURRENT_SOURCE_DIR}/parser
  ${CMAKE_CURRENT_SOURCE_DIR}/kaleidoscopejit)

file(GLOB JIT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/kaleidoscopejit/*.cpp)
file(GLOB AST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/ast/*.cpp)
file(GLOB LOGGER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/logger/*.cpp)
file(GLOB LEXER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/lexer/*.cpp)
file(GLOB PARSER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/parser/*.cpp)

# excluding included jit file
file(GLOB UTIL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cpp)
set(EXCLUDE_FILES "kaleidoscop_jit_include.cpp")
foreach(EXCLUDE_FILE ${EXCLUDE_FILES})
  list(REMOVE_ITEM UTILS_SOURCES ${EXCLUDE_ITEM})
endforeach()

# add_library(ast_lib SHARED ${AST_SOURCES}) add_library(ast_lib INTERFACE)
# target_link_directories(ast_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/ast)

add_library(lexer SHARED ${LEXER_SOURCES})
add_library(logger SHARED ${LOGGER_SOURCES})
add_library(parser STATIC ${PARSER_SOURCES})
add_library(ast SHARED ${AST_SOURCES})
add_library(util SHARED ${UTIL_SOURCES})
add_library(jit SHARED ${JIT_SOURCES})

add_executable(kaleidoscope main.cpp)
add_compile_options(kaleidoscope PRIVATE -g)
target_link_libraries(kaleidoscope PRIVATE logger lexer parser ast util jit)
