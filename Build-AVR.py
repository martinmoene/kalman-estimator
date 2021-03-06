#!/usr/bin/env python

#
# Copyright 2018 by Martin Moene
#
# https://github.com/martinmoene/kalman-estimator
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

import os

cmd = 'cd build && cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-avr-gcc.cmake -G "Unix Makefiles" .. && make'

print(cmd)

os.system( cmd )
