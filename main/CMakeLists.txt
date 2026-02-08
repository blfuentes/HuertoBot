FILE(GLOB_RECURSE app_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
FILE(GLOB_RECURSE lib_sources ${CMAKE_SOURCE_DIR}/lib/*.c)

idf_component_register(
	SRCS ${app_sources} ${lib_sources}
	INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/include"
	              "${CMAKE_SOURCE_DIR}/lib/ads1115"
	              "${CMAKE_SOURCE_DIR}/lib/ads1115/hal"
)
