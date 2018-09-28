# Copyright 2018 by Martin Moene
#
# https://github.com/martinmoene/kalman-estimator
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required( VERSION 3.5 )

project( example_desktop LANGUAGES CXX )

# Target default options and definitions:

set( OPTIONS "" )
set( DEFINITIONS "" )

if( MSVC )
    set( OPTIONS ${OPTIONS} -permissive- )
endif()

# Sources (.cpp) and their base names:

set( SOURCES
    biquad.e.cpp
    biquad-cascade.e.cpp
    chebyshev-design.e.cpp
    covariance-error.e.cpp
    kalman-sim.e.cpp
    matrix.e.cpp
    range.e.cpp
)

# Function to create a target:

function( make_target source )
    string( REPLACE ".cpp" "" target "${source}" )
    add_executable            ( ${target} ${source}  )
    target_compile_options    ( ${target} PRIVATE ${OPTIONS} )
    target_compile_definitions( ${target} PRIVATE ${DEFINITIONS} )
    target_link_libraries     ( ${target} PRIVATE kalman-estimator )
    set_property       ( TARGET ${target} PROPERTY CXX_STANDARD 17 )
    set_property(        TARGET ${target} PROPERTY CXX_STANDARD_REQUIRED ON )
    set_property       ( TARGET ${target} PROPERTY CXX_EXTENSIONS OFF )
endfunction()

# Targets:

enable_testing()

foreach( target ${SOURCES} )
    make_target( ${target} )
endforeach()

# end of file
