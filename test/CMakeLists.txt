# Copyright 2018 by Martin Moene
#
# https://github.com/martinmoene/kalman-estimator
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required( VERSION 3.5 )

project( test LANGUAGES CXX )

# Configure kalman-estimator for testing:

set( OPTIONS "" )
set( DEFINITIONS "" )

# -----------------------------------------------------------
#
if( AVR )

    # Sources (.cpp) and their base names:

    set( SOURCES
        main-avr.t.cpp
    )

    # Function to create a target:

    function( make_target source )
        string( REPLACE ".cpp" "" target "${source}" )
        add_avr_executable            ( ${target} ${source}  )
#        avr_target_compile_options    ( ${target} PRIVATE ${OPTIONS} )
#        avr_target_compile_definitions( ${target} PRIVATE ${DEFINITIONS} )
#        avr_target_link_libraries     ( ${target} PRIVATE kalman-estimator )
    endfunction()

    # Targets:

    enable_testing()

    foreach( target ${SOURCES} )
        make_target( ${target} )
    endforeach()

else( AVR )

# -----------------------------------------------------------
# PC

set( MAIN_BASE main )
set( HDRNAME   matrix.hpp    )
set( SOURCES   ${MAIN_BASE}.t.cpp core.t.cpp stdcpp.t.cpp biquad.t.cpp biquad-cascade.t.cpp fixed-point.t.cpp matrix.t.cpp )

set( HDRDIR  ${PROJECT_SOURCE_DIR}/../include )
#set( HDRPATH ${HDRDIR}/${HDRNAME} )
set( HDRPATH "" )

set ( LESTDIR "lest")

set( HAS_STD_FLAGS  FALSE )
set( HAS_CPP98_FLAG FALSE )
set( HAS_CPP11_FLAG FALSE )
set( HAS_CPP14_FLAG FALSE )
set( HAS_CPP17_FLAG FALSE )
set( HAS_CPP20_FLAG FALSE )
set( HAS_CPPLATEST_FLAG FALSE )

if( MSVC )
    set( HAS_STD_FLAGS TRUE )

    set( OPTIONS     -W3 -EHsc -permissive- )
    set( DEFINITIONS -Dlest_FEATURE_AUTO_REGISTER=1 -D_SCL_SECURE_NO_WARNINGS -DNOMINMAX )

    if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.00 )
        set( HAS_CPP14_FLAG TRUE )
        set( HAS_CPPLATEST_FLAG TRUE )
    endif()
    if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.11 )
        set( HAS_CPP17_FLAG TRUE )
    endif()

elseif( CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" )

    set( HAS_STD_FLAGS  TRUE )
    set( HAS_CPP98_FLAG TRUE )

    set( OPTIONS     -Wall -Wno-missing-braces -fno-elide-constructors )
    set( DEFINITIONS -Dlest_FEATURE_AUTO_REGISTER=1 )

    # GNU: available -std flags depends on version
    if( CMAKE_CXX_COMPILER_ID MATCHES "GNU" )
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8.0 )
            set( HAS_CPP11_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9.2 )
            set( HAS_CPP14_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.1.0 )
            set( HAS_CPP17_FLAG TRUE )
        endif()
    endif()

    # Clang: available -std flags depends on version
    if( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.3.0 )
            set( HAS_CPP11_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.4.0 )
            set( HAS_CPP14_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0.0 )
            set( HAS_CPP17_FLAG TRUE )
        endif()
    endif()

elseif( CMAKE_CXX_COMPILER_ID MATCHES "Intel" )
# as is
else()
# as is
endif()

# enable MS C++ Core Guidelines checker if MSVC:

function( enable_msvs_guideline_checker target )
    if( MSVC )
        set_target_properties( ${target} PROPERTIES
        VS_GLOBAL_EnableCppCoreCheck true
        VS_GLOBAL_CodeAnalysisRuleSet CppCoreCheckRules.ruleset
        VS_GLOBAL_RunCodeAnalysis true )
    endif()
endfunction()

# make target, compile for given standard if specified:

function( make_target target std )
    add_executable            ( ${target} ${SOURCES} ${HDRPATH} )
    target_compile_options    ( ${target} PRIVATE ${OPTIONS} )
    target_compile_definitions( ${target} PRIVATE ${DEFINITIONS} )
    target_include_directories( ${target} PRIVATE ${HDRDIR} ${LESTDIR} )
    if( std )
        if( MSVC )
            target_compile_options( ${target} PRIVATE -std:c++${std} )
        else()
            # Necessary for clang 3.x:
            target_compile_options( ${target} PRIVATE -std=c++${std} )
            # Ok for clang 4 and later:
            # set_property( TARGET ${target} PROPERTY CXX_STANDARD ${std} )
            # set_property( TARGET ${target} PROPERTY CXX_STANDARD_REQUIRED ON )
            # set_property( TARGET ${target} PROPERTY CXX_EXTENSIONS OFF )
        endif()
    endif()
endfunction()

# add generic executable, unless -std flags can be specified:

if( HAS_STD_FLAGS )
    if( HAS_CPP17_FLAG )
        make_target( ${MAIN_BASE}-cpp17.t 17 )
        enable_msvs_guideline_checker( ${MAIN_BASE}-cpp17.t )
    endif()

    if( HAS_CPPLATEST_FLAG )
        make_target( ${MAIN_BASE}-cpplatest.t latest )
    endif()
endif()

# configure unit tests via CTest:

enable_testing()

if( HAS_STD_FLAGS )
    if( HAS_CPP17_FLAG )
        add_test( NAME test-cpp17     COMMAND ${MAIN_BASE}-cpp17.t )
    endif()
    if( HAS_CPPLATEST_FLAG )
        add_test( NAME test-cpplatest COMMAND ${MAIN_BASE}-cpplatest.t )
    endif()
else()
    add_test(     NAME test           COMMAND ${MAIN_BASE}.t --pass )
    add_test(     NAME list_version   COMMAND ${MAIN_BASE}.t --version )
    add_test(     NAME list_tags      COMMAND ${MAIN_BASE}.t --list-tags )
    add_test(     NAME list_tests     COMMAND ${MAIN_BASE}.t --list-tests )
endif()

endif( AVR )

# end of file
