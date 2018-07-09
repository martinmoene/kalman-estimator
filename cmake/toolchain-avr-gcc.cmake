# Copyright 2018 by Martin Moene
#
# https://github.com/martinmoene/kalman-estimator
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# Inspired on:
# C++Now 2018: Michael Caisse. "Modern C++ in Embedded Systems"
# - https://www.youtube.com/watch?v=c9Xt6Me3mJ4
# - https://github.com/boostcon/cppnow_presentations_2018/blob/master/05-10-2018_thursday/modern_cpp_in_an_embedded_world__michael_caisse__cpnow_05102018.pdf
# Matthias Kleemann. cmake-avr - a cmake toolchain for AVR projects
# - cmake toolchain for AVR, https://github.com/mkleemann/cmake-avr

# Indicate compilation for AVR:
set( AVR 1 )

# Options:
option( FILENAME_WITH_MCU_TYPE "Add the MCU type to the target file name." ON)

# Default MCU choices:

if( NOT AVR_MCU )
    set( AVR_MCU  atmega328p CACHE STRING "Default MCU : atmega328p")
endif()

if( NOT AVR_FMCU )
    set( AVR_FMCU 16000000L CACHE STRING "Default FMCU: 16 MHz")
endif()

if (NOT AVR_PROGRAMMER )
    set( AVR_PROGRAMMER usbtiny CACHE STRING "Default programmer: usbtiny")
endif()

if( NOT AVR_SIZE_ARGS )
    set( AVR_SIZE_ARGS "" )
endif()

# C++ related:

if( NOT AVR_CXX_STD )
    set( AVR_CXX_STD "c++17" )
endif()

if( NOT AVR_CXX_OPT )
    set( AVR_CXX_OPT "-O2" )
endif()

# TODO: yet to find out how to get include directories
# for use with compilation to assembly

if( NOT AVR_CXX_INCLUDE )
    set( AVR_CXX_INCLUDE "${CMAKE_SOURCE_DIR}/include" )
endif()

# Locate programs:

find_program( AVR_CC        NAMES avr-gcc     HINTS $ENV{AVR_GCC_ROOT}/bin )
find_program( AVR_CXX       NAMES avr-g++     HINTS $ENV{AVR_GCC_ROOT}/bin )
find_program( AVR_OBJCOPY   NAMES avr-objcopy HINTS $ENV{AVR_GCC_ROOT}/bin )
find_program( AVR_SIZE_TOOL NAMES avr-size    HINTS $ENV{AVR_GCC_ROOT}/bin )
find_program( AVR_OBJDUMP   NAMES avr-objdump HINTS $ENV{AVR_GCC_ROOT}/bin )
find_program( AVR_DUDE      NAMES avrdude     HINTS $ENV{AVR_GCC_ROOT}/bin )

# Mandatory toolchain variables:
# The Generic system is used with CMake to specify bare metal:

set( CMAKE_SYSTEM_NAME      Generic )
set( CMAKE_SYSTEM_PROCESSOR avr )
set( CMAKE_CXX_COMPILER_ID  GNU )
set( CMAKE_C_COMPILER     "${AVR_CC}" )
set( CMAKE_CXX_COMPILER   "${AVR_CXX}" )

#set( CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY )

# Search for programs in the build host directories:

SET( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )

# For libraries and headers in the target directories:

SET( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
SET( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )
SET( CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY )

# Optionally append MCU type to filename:

if( FILENAME_WITH_MCU_TYPE )
   set( FILENAME_MCU_PART "-${AVR_MCU}")
else()
   set( FILENAME_MCU_PART "")
endif()

#
# add_avr_executable( exe_name )
#
# Creates exe_name.elf etc.
# Use target_link_libraries( exe_name.elf ...).
#
function( add_avr_executable EXECUTABLE_NAME )

    # Check for filenames:
    if( NOT ARGN )
        message( FATAL_ERROR "No source files given for ${EXECUTABLE_NAME}.")
    endif()

#    message( STATUS "NOTICE: add_avr_executable ${EXECUTABLE_NAME}.")

    # File names:
    set( elf_file     ${EXECUTABLE_NAME}${FILENAME_MCU_PART}.elf )
    set( hex_file     ${EXECUTABLE_NAME}${FILENAME_MCU_PART}.hex )
    set( map_file     ${EXECUTABLE_NAME}${FILENAME_MCU_PART}.map )
    set( eeprom_image ${EXECUTABLE_NAME}${FILENAME_MCU_PART}.eep )

    # Generate elf file:
    add_executable(
        ${elf_file} EXCLUDE_FROM_ALL ${ARGN}
    )

    target_include_directories(
        ${elf_file} PRIVATE ${AVR_CXX_INCLUDE}
    )

    # Compilation and linking:
    set_target_properties(
        ${elf_file} PROPERTIES
        COMPILE_DEFINITIONS "F_CPU_HZ=${AVR_FMCU}"
        COMPILE_FLAGS       "-mmcu=${AVR_MCU} -flto -std=${AVR_CXX_STD} -Wall ${AVR_CXX_OPT} -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD"
        LINK_FLAGS          "-mmcu=${AVR_MCU} -flto -fuse-linker-plugin -Wl,--gc-sections -mrelax -Wl,-Map,${map_file}"
    )

    # Generate assembly:
    set( asm_list "" )
    foreach( f ${ARGN})
        get_filename_component(ext  ${f} EXT )
        get_filename_component(name ${f} NAME_WE )
        if( ext MATCHES "\.cpp|\.cxx" )
            set(  cpp_file "${CMAKE_CURRENT_SOURCE_DIR}/${f}" )
            set(  asm_file "${name}${FILENAME_MCU_PART}.s" )
            list( APPEND asm_list ${asm_file} )
            add_custom_command(
                OUTPUT  ${asm_file}
                COMMAND ${AVR_CXX} -mmcu=${AVR_MCU} -DF_CPU_HZ=${AVR_FMCU} -std=${AVR_CXX_STD} -Wall ${AVR_CXX_OPT} -I${AVR_CXX_INCLUDE} -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -S -o "${asm_file}" "${cpp_file}"
                DEPENDS ${f}
            )
        endif()
    endforeach()
#    message( STATUS "NOTICE: generate assembly for: ${asm_list}")

   # Generate hex file:
    add_custom_command(
        OUTPUT  ${hex_file}
        COMMAND ${AVR_OBJCOPY} -O ihex -R .eeprom  ${elf_file} ${hex_file}
        COMMAND ${AVR_SIZE_TOOL} ${AVR_SIZE_ARGS} ${elf_file}
        DEPENDS ${elf_file}
    )

    # Generate eeprom:
    add_custom_command(
        OUTPUT  ${eeprom_image}
        COMMAND ${AVR_OBJCOPY} -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0  ${elf_file} ${eeprom_image}
        DEPENDS ${elf_file}
    )

    # Add dependencies:
    add_custom_target(
        ${EXECUTABLE_NAME} ALL DEPENDS ${asm_list} ${hex_file} ${eeprom_image}
    )

    set_target_properties(
        ${EXECUTABLE_NAME} PROPERTIES OUTPUT_NAME "${elf_file}"
    )

    # Clean:
    get_directory_property(
        clean_files ADDITIONAL_MAKE_CLEAN_FILES
    )

    set_directory_properties(
        PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${map_file}"
    )

    # Upload with avrdude; use with target upload_<target>:
    add_custom_target(
        upload_${EXECUTABLE_NAME}
        ${AVR_DUDE} -v -p ${AVR_MCU} -c ${AVR_PROGRAMMER} -U flash:w:${hex_file}:i
        DEPENDS ${hex_file}
        COMMENT "Uploading ${hex_file} to ${AVR_MCU} using ${AVR_PROGRAMMER}"
    )

endfunction( add_avr_executable )

#
# TODO: implement
# add_avr_library( lib_name )
# avr_target_link_libraries( EXECUTABLE_TARGET )
# avr_target_include_directories( EXECUTABLE_TARGET )
# avr_target_compile_definitions( EXECUTABLE_TARGET )
# avr_target_compile_options( EXECUTABLE_TARGET )
#
function( add_avr_library LIBRARY_NAME )
    message( FATAL_ERROR "NOTICE: implement add_avr_library ${LIBRARY_NAME}.")
endfunction()

function( avr_target_link_libraries EXECUTABLE_TARGET)
    message( FATAL_ERROR "NOTICE: implement avr_target_link_libraries ${EXECUTABLE_TARGET}.")
endfunction()

function( avr_target_include_directories EXECUTABLE_TARGET)
    if( NOT ARGN )
        message( FATAL_ERROR "No include directories to add to ${EXECUTABLE_TARGET}.")
    endif()

    get_target_property( TARGET_LIST ${EXECUTABLE_TARGET} OUTPUT_NAME )
    set( extra_args ${ARGN} )

    target_include_directories( ${TARGET_LIST} PRIVATE "${extra_args}" )
endfunction()

function( avr_target_compile_definitions EXECUTABLE_TARGET)
    message( FATAL_ERROR "NOTICE: implement avr_target_compile_definitions ${EXECUTABLE_TARGET}.")
endfunction()

function( avr_target_compile_options EXECUTABLE_TARGET)
    message( FATAL_ERROR "NOTICE: implement avr_target_compile_options ${EXECUTABLE_TARGET}.")
endfunction()

if( CMAKE_C_COMPILER_FORCED )
    # The compiler configuration was forced by the user.
    # Assume the user has configured all compiler information.
    set( CMAKE_C_COMPILER_WORKS TRUE )
    return()
endif()

# end of file
