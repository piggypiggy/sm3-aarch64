cmake_minimum_required(VERSION 3.1.0)

project(sm3-aarch64 LANGUAGES C ASM)

option(BUILD_STATIC "build static library" ON)
option(BUILD_SHARED "build shared library" ON)
option(USE_ASAN "use AddressSanitizer" OFF)

if(${BUILD_STATIC} STREQUAL "OFF")
    unset(${BUILD_STATIC})
endif()

if(${BUILD_SHARED} STREQUAL "OFF")
    unset(${BUILD_SHARED})
endif()

if((NOT BUILD_STATIC) AND (NOT BUILD_SHARED))
    message(FATAL_ERROR "static or shared or both")
endif()

# default : release
if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

# Disable in-source builds
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message("BUILD_DIR : ${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed.")
endif("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")

include(CheckTypeSize)
include(CheckIncludeFiles)

# debug and release
if(NOT CMAKE_CONFIGURATION_TYPES)
    get_property(HAVE_MULTI_CONFIG_GENERATOR GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    # Set default configuration types for multi-config generators
    if(HAVE_MULTI_CONFIG_GENERATOR)
        set(CMAKE_CONFIGURATION_TYPES "Release;Debug")
    endif()
endif()

# arch
if(CMAKE_SYSTEM_PROCESSOR MATCHES "(arm64|aarch64)")
    set(ARCH_AARCH64 1)
else()
    message(FATAL_ERROR "aarch64 only...")
endif()

find_program(PERL perl)
if(NOT PERL)
    message(FATAL_ERROR "perl not found")
endif()
message("${PERL}")

# endianess
include(TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
if(IS_BIG_ENDIAN)
    set(ORDER 0) # config
else()
    set(ORDER 1) # config
endif()
set(ENDIANESS "O.O") # config

# compiler flags
set(EXTRA_C_FLAGS -Wall -Wextra -pedantic)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(EXTRA_C_FLAGS ${EXTRA_C_FLAGS} -fomit-frame-pointer)
else()
    set(EXTRA_C_FLAGS ${EXTRA_C_FLAGS})
endif()

if(USE_ASAN)
    set(SANITIZER_FLAG -fsanitize=address)
    set(EXTRA_C_FLAGS ${EXTRA_C_FLAGS} ${SANITIZER_FLAG})
endif()

set(SM3_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(OUTPUT_DIRECTORY ${SM3_DIR}/bin)

add_custom_command(OUTPUT ${SM3_DIR}/sm3-aarch64.s
                   COMMAND ${PERL} ${SM3_DIR}/sm3-aarch64.pl linux64 ${ORDER} ${SM3_DIR}/sm3-aarch64.s
                   DEPENDS ${SM3_DIR}/sm3-aarch64.pl
)

add_custom_command(OUTPUT ${SM3_DIR}/utils-aarch64.s
                   COMMAND ${PERL} ${SM3_DIR}/utils-aarch64.pl linux64 ${SM3_DIR}/utils-aarch64.s
                   DEPENDS ${SM3_DIR}/utils-aarch64.pl
)

# generate config.h
configure_file(${SM3_DIR}/config.h.in ${SM3_DIR}/config.h @ONLY)

set(sm3_src
    ${SM3_DIR}/sm3.h
    ${SM3_DIR}/sm3.c
    ${SM3_DIR}/sm3-aarch64.s
    ${SM3_DIR}/utils-aarch64.s
)

# header file directory
include_directories(${SM3_DIR}/)

if(BUILD_STATIC)
    add_library(sm3_static STATIC ${sm3_src})
    target_compile_definitions(sm3_static PRIVATE BUILD_STATIC)
    target_compile_options(sm3_static PRIVATE ${EXTRA_C_FLAGS})
    target_link_libraries(sm3_static ${SANITIZER_FLAG})
    # output directory
    set_target_properties(sm3_static PROPERTIES
                          ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})
    # rename
    set_target_properties(sm3_static PROPERTIES OUTPUT_NAME "sm3-aarch64")
    # install library
    INSTALL(TARGETS sm3_static ARCHIVE DESTINATION lib)
endif()

if(BUILD_SHARED)
    add_library(sm3_shared SHARED ${sm3_src})
    # PIC
    set_property(TARGET sm3_shared PROPERTY POSITION_INDEPENDENT_CODE 1)
    target_compile_definitions(sm3_shared PRIVATE BUILD_SHARED)
    target_compile_options(sm3_shared PRIVATE ${EXTRA_C_FLAGS})
    target_link_libraries(sm3_shared ${SANITIZER_FLAG})
    # output directory
    set_target_properties(sm3_shared PROPERTIES
                          LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})
    # rename
    set_target_properties(sm3_shared PROPERTIES OUTPUT_NAME "sm3-aarch64")
    # install library
    INSTALL(TARGETS sm3_shared LIBRARY DESTINATION lib)
endif()

# install headers
INSTALL(FILES ${SM3_DIR}/sm3.h DESTINATION include)

################# BUILD BENCH #################
project(sm3_bench LANGUAGES C)

set(bench_src
    ${SM3_DIR}/bench.h
    ${SM3_DIR}/bench.c
    ${SM3_DIR}/test.c
    ${SM3_DIR}/test.h
)

add_executable(sm3_bench ${bench_src})

# prefer static library
if(BUILD_STATIC)
    set(bench_dep_lib sm3_static)
    target_compile_definitions(sm3_bench PRIVATE BUILD_STATIC)
elseif(BUILD_SHARED)
    set(bench_dep_lib sm3_shared)
    target_compile_definitions(sm3_bench PRIVATE BUILD_SHARED)
endif()

target_compile_options(sm3_bench PRIVATE ${EXTRA_C_FLAGS})
target_link_libraries(sm3_bench ${bench_dep_lib} ${SANITIZER_FLAG})

################# BUILD TEST #################
project(sm3_test LANGUAGES C)
enable_testing()

set(test_src
    ${SM3_DIR}/sm3_test.c
    ${SM3_DIR}/test.c
    ${SM3_DIR}/test.h
    ${SM3_DIR}/sm3.h
)

add_executable(sm3_test ${test_src})
# prefer shared library
if(BUILD_SHARED)
    set(test_dep_lib sm3_shared)
    target_compile_definitions(sm3_test PRIVATE BUILD_SHARED)
elseif(BUILD_STATIC)
    set(test_dep_lib sm3_static)
    target_compile_definitions(sm3_test PRIVATE BUILD_STATIC)
endif()

target_compile_options(sm3_test PRIVATE ${EXTRA_C_FLAGS})
target_link_libraries(sm3_test ${test_dep_lib} ${SANITIZER_FLAG})
add_test(SM3_TEST sm3_test)

##########################################################
set(BUILD_FLAGS CMAKE_C_FLAGS_${CMAKE_BUILD_TYPE})
string(TOUPPER ${BUILD_FLAGS} BUILD_FLAGS)
message("================ summary ================")
message("os               : ${CMAKE_SYSTEM_NAME}")
message("processor        : ${CMAKE_SYSTEM_PROCESSOR}")
message("build type       : ${CMAKE_BUILD_TYPE}")
message("C compiler       : ${CMAKE_C_COMPILER}")
message("C compiler flags : ${CMAKE_C_FLAGS} ${${BUILD_FLAGS}} ${EXTRA_C_FLAGS}")
message("assembler        : ${CMAKE_ASM_COMPILER}\n")
