cmake_minimum_required(VERSION 3.5)

set(LIBRARY_NAME    satsolver)
set(EXECUTABLE_NAME sats)

#
# ------- Coverage Options -----------------------------------------------
#

option(WITH_COVERAGE "If YES, build the debug executable with coverage." NO)

SET(COV_FLAGS_C     "-fprofile-arcs -ftest-coverage")
SET(COV_FLAGS_LINK  "-fprofile-arcs -ftest-coverage")

#
# -------- Compilation Flags ---------------------------------------------
#


SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -Wall -W")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_C_FLAGS_RELEASE}")

if( ${WITH_COVERAGE} )

message(STATUS "Building with coverage flags set.")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${COV_FLAGS_C}")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_C_FLAGS_RELEASE} ${COV_FLAGS_LINK}")

else()

message(STATUS "NOT building with coverage.")

endif()


SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")

message(STATUS "Parser Build Flags Debug:   ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "Parser Link Flags Debug:   ${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
message(STATUS "Parser Build Flags Release: ${CMAKE_C_FLAGS_RELEASE}")
message(STATUS "Parser Link Flags Release: ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")

#
# --------- Flex and Bison Processing ------------------------------------
#


set(FLEX_IN     ${CMAKE_CURRENT_SOURCE_DIR}/parser/sat-expression-scanner.l)
set(FLEX_OUT_C  ${CMAKE_CURRENT_BINARY_DIR}/sat-expression-scanner.c)
set(FLEX_OUT_H  ${CMAKE_CURRENT_BINARY_DIR}/sat-expression-scanner.h)

set(BISON_IN    ${CMAKE_CURRENT_SOURCE_DIR}/parser/sat-expression-parser.y)
set(BISON_OUT_C ${CMAKE_CURRENT_BINARY_DIR}/sat-expression-parser.c)
                
set_source_files_properties(
    ${FLEX_OUT_C} ${FLEX_OUT_H} ${BISON_OUT_C}
    PROPERTIES GENERATED TRUE
)

add_custom_command(
    OUTPUT  ${FLEX_OUT_C} ${FLEX_OUT_H}
    COMMAND ${FLEX_EXECUTABLE} -o ${FLEX_OUT_C} --header-file=${FLEX_OUT_H} ${FLEX_IN}
    COMMENT "Generating Lexer"
    VERBATIM
    DEPENDS ${FLEX_IN}
)

add_custom_command(
    OUTPUT  ${BISON_OUT_C}
    COMMAND ${BISON_EXECUTABLE} -o ${BISON_OUT_C} ${BISON_IN}
    COMMENT "Generating Parser"
    DEPENDS ${FLEX_OUT_H} ${BISON_IN}
)


#
# -------- Library and Executable ----------------------------------------
#

include_directories(AFTER ${CMAKE_CURRENT_SOURCE_DIR}/parser/)
include_directories(AFTER ${CMAKE_CURRENT_SOURCE_DIR}/)

set(LIBRARY_SRC ${CMAKE_CURRENT_SOURCE_DIR}/imp-matrix.c
                ${FLEX_OUT_C}
                ${BISON_OUT_C}
                ${CMAKE_CURRENT_SOURCE_DIR}/sat-expression.c
)

add_library(${LIBRARY_NAME} ${LIBRARY_SRC})
target_link_libraries(${LIBRARY_NAME} m)

set(CMAKE_C_OUTPUT_EXTENSION_REPLACE 1)

add_executable(${EXECUTABLE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.c)
target_link_libraries(${EXECUTABLE_NAME} ${LIBRARY_NAME})



