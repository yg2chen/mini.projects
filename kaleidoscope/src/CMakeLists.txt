include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/lexer
	${CMAKE_CURRENT_SOURCE_DIR}/logger
	${CMAKE_CURRENT_SOURCE_DIR}/ast
	${CMAKE_CURRENT_SOURCE_DIR}/utils
	${CMAKE_CURRENT_SOURCE_DIR}/parser
)

file(GLOB AST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/ast/*.cpp)
file(GLOB LOGGER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/logger/*.cpp)
file(GLOB LEXER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/lexer/*.cpp)
file(GLOB PARSER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/parser/*.cpp)
file(GLOB UTIL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cpp)


# add_library(ast_lib SHARED ${AST_SOURCES})
# add_library(ast_lib INTERFACE)
# target_link_directories(ast_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/ast)

add_library(ast_lib SHARED ${AST_SOURCES})

add_library(lexer_lib SHARED ${LEXER_SOURCES})

add_library(logger_lib SHARED ${LOGGER_SOURCES})

add_library(parser_lib STATIC ${PARSER_SOURCES})

add_library(util_lib SHARED ${UTIL_SOURCES})



add_executable(kaleidoscope main.cpp)
add_compile_options(kaleidoscope PRIVATE -g -Wall)
target_link_libraries(kaleidoscope PRIVATE
	logger_lib
	lexer_lib
	parser_lib
	ast_lib
	util_lib
)

