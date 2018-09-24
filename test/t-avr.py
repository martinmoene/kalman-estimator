#!/usr/bin/env python

# Copyright 2018 by Martin Moene
#
# https://github.com/martinmoene/kalman-estimator
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# Compile for various combinations of numeric type,
# updating/fixed Kalman gain and optimization.

from __future__ import print_function

import os

num_type = 'double'
num_type = 'fp32_t'

std      = 'c++17'
opt      = '-O2'
mcu      = 'atmega328p'
fcpu     = '16000000L'
filename = 'main-avr.t.cpp'
verbose  = '-vv'

cmd = 'python ../script/avr-gcc.py {verbose} -std={std} {opt} -mmcu={mcu} -fcpu={fcpu} -DKE_NUMERIC_TYPE={nt} -I../include {filename}'

os.system( cmd.format(nt=num_type, verbose=verbose, std=std, opt=opt, mcu=mcu, fcpu=fcpu, filename=filename) )
