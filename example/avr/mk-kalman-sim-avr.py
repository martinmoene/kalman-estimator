#!/usr/bin/env python

#
# Copyright 2018 by Martin Moene
#
# https://github.com/martinmoene/kalman-estimator
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

import os

nt       = 'double'
nt       = 'fp32_t'

std      = 'c++17'
opt      = '-O2'
mcu      = 'atmega328p'
fcpu     = '16000000L'
filename = 'kalman-sim-avr.cpp'
verbose  = '-vv'

tpl = 'python ../../script/avr-gcc.py {verbose} -std={std} {opt} -mmcu={mcu} -fcpu={fcpu} -DKE_NUMERIC_TYPE={nt} -I../include {filename}'
cmd = tpl.format(nt=nt, verbose=verbose, std=std, opt=opt, mcu=mcu, fcpu=fcpu, filename=filename)

print(     cmd )
os.system( cmd )
