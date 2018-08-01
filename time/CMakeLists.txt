# Copyright 2018 by Martin Moene
#
# https://github.com/martinmoene/kalman-estimator
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required( VERSION 3.5 )

project( time LANGUAGES CXX)

set( BASENAME avr-kalman-time )
set( TARGET   ${BASENAME} )
set( SOURCES  ${BASENAME}.cpp )
set( HDRDIR   ${PROJECT_SOURCE_DIR}/../include )
set( HEADERS  ${HDRDIR}/num/kalman.hpp ${HDRDIR}/num/matrix.hpp ${HDRDIR}/num/fixed-point.hpp )

if( AVR )

add_avr_executable(             ${TARGET} ${SOURCES} ${HEADERS} )
#avr_target_include_directories( ${TARGET} ${HDRDIR} )

else()

message( STATUS "NOTICE: ${TARGET} must be compiled with AVR-GCC.")

endif()
