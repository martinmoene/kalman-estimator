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

num_types     = ('double', 'fp32_t')
gain_update   = ((1,'updat'), (0,'fixed'))
optimizations = ('Os', 'O2')

std      = 'c++17'
mcu      = 'atmega328p'
fcpu     = '16000000L'
filename = 'kalman-time-avr.cpp'
verbose  = '-vv'

def remove_ext(path):
    return os.path.splitext(path)[0]

basename = remove_ext(filename)

cmd = 'python ../script/avr-gcc.py {verbose} -std={std} -{opt} -mmcu={mcu} -DKE_NUMERIC_TYPE={nt} -DKE_UPDATE_KALMAN_GAIN={update} -I../include --output {outname} {filename}'

for num_type in num_types:
    for update,text in gain_update:
        for optimization in optimizations:
            outname = '{basename}-{nt}-{upd}-{opt}'.format( basename=basename, nt=num_type, upd=text, opt=optimization )
            os.system( cmd.format(nt=num_type, update=update, opt=optimization, verbose=verbose, std=std, mcu=mcu, fcpu=fcpu, outname=outname, filename=filename) )
