#!/usr/bin/env python

#
# Copyright 2018 by Martin Moene
#
# https://github.com/martinmoene/kalman-estimator
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

import os

blink_ms = '200'
#blink_ms = '0'

std      = 'c++17'
opt      = '-O2'
mcu      = 'atmega328p'
fcpu     = '16000000L'
filename = 'fade-led-avr.cpp'
verbose  = '-vv'

cmd = 'python ../script/avr-gcc.py {verbose} -std={std} {opt} -mmcu={mcu} -fcpu={fcpu} -Dled_FEATURE_BLINK_MS={ms} {filename}'

os.system( cmd.format(verbose=verbose, std=std, opt=opt, mcu=mcu, fcpu=fcpu, ms=blink_ms, filename=filename) )
