cmake_minimum_required(VERSION 3.5)

project(sat-solver)

enable_testing()

message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

set(LIBRARY_NAME    satsolver)
set(EXECUTABLE_NAME sats)

# ------------------------------------------------------------------------

option(WITH_COVERAGE "If YES, build the debug executable with coverage." NO)

SET(COV_FLAGS_C     "-fprofile-arcs -ftest-coverage")
SET(COV_FLAGS_LINK  "-fprofile-arcs -ftest-coverage")

# ------------------------------------------------------------------------

SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -Wall -W")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_C_FLAGS_RELEASE}")

if( ${WITH_COVERAGE} )

message(STATUS "Building with coverage flags set.")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${COV_FLAGS_C} -DVERILOG_PARSER_COVERAGE_ON")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_C_FLAGS_RELEASE} ${COV_FLAGS_LINK}")

else()

message(STATUS "NOT building with coverage.")

endif()


SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")

message(STATUS "Parser Build Flags Debug:   ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "Parser Link Flags Debug:   ${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
message(STATUS "Parser Build Flags Release: ${CMAKE_C_FLAGS_RELEASE}")
message(STATUS "Parser Link Flags Release: ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")

# ------------------------------------------------------------------------

set(LIBRARY_SRC imp-matrix.c
)

add_library(${LIBRARY_NAME} ${LIBRARY_SRC})

set(CMAKE_C_OUTPUT_EXTENSION_REPLACE 1)

add_executable(${EXECUTABLE_NAME} main.c)
target_link_libraries(${EXECUTABLE_NAME} ${LIBRARY_NAME})
