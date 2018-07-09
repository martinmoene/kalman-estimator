# Copyright 2018 by Martin Moene
#
# https://github.com/martinmoene/kalman-estimator
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required( VERSION 3.5 )

# At default, enable building and performing of tests and building of examples:

option( KE_OPT_BUILD_TEST    "Build and perform kalman-estimator tests" ON )
option( KE_OPT_BUILD_EXAMPLE "Build kalman-estimator examples" ON )
option( KE_OPT_BUILD_TIME    "Build kalman-estimator timing" ON )

project( kalman_estimator LANGUAGES CXX )

set( package_name "kalman-estimator" )

set( include_source_dir "${PROJECT_SOURCE_DIR}/include" )

# Interface library:

add_library(
    ${package_name} INTERFACE )

target_include_directories(
    ${package_name} INTERFACE "$<BUILD_INTERFACE:${include_source_dir}>" )

enable_testing()

if ( KE_OPT_BUILD_TEST )
    add_subdirectory( test )
endif()

if ( KE_OPT_BUILD_EXAMPLE )
    add_subdirectory( example )
endif()

if ( KE_OPT_BUILD_TIME )
    add_subdirectory( time )
endif()

# end of file
