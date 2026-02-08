# Auto-clone BME280 library if not present
set(BME280_DIR "${CMAKE_SOURCE_DIR}/lib/bme280")

if(NOT EXISTS "${BME280_DIR}/bme280.c")
    find_package(Git REQUIRED)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} clone --depth 1
                https://github.com/boschsensortec/BME280_SensorAPI.git
                "${BME280_DIR}"
        RESULT_VARIABLE GIT_RESULT
    )
    if(NOT GIT_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to clone BME280_SensorAPI")
    endif()
endif()

FILE(GLOB_RECURSE app_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
FILE(GLOB_RECURSE lib_sources ${CMAKE_SOURCE_DIR}/lib/*.c)
list(FILTER lib_sources EXCLUDE REGEX ".*/examples/.*")

idf_component_register(
	SRCS ${app_sources} ${lib_sources}
	INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/include"
	              "${CMAKE_SOURCE_DIR}/lib/ads1115"
	              "${CMAKE_SOURCE_DIR}/lib/ads1115/hal"
	              "${BME280_DIR}"
)
