#!/usr/bin/env python

#
# Copyright 2018 by Martin Moene
#
# https://github.com/martinmoene/kalman-estimator
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

from __future__ import print_function

import argparse
import os
import re
import sys

# Configuration:

# atmega328: avr5 - "Enhanced" devices with 16 KiB up to 64 KiB of program memory.

mcu      = 'atmega328p'

# Derived configuration:

cmd_upl = '{avrdude} -v -p{mcu} -cusbtiny -Uflash:w:{filename}:i'

# End configuration

# AVR-GCC programs:

avr_gcc_root = os.environ['AVR_GCC_ROOT']

if len( avr_gcc_root ) == 0:
    print("Expected environment variable 'AVR_GCC_ROOT' to be set to root of AVR-GCC.")
    sys.exit()

#print("avr_gcc_root :{avr_gcc_root}".format(avr_gcc_root=avr_gcc_root) )

#cpp     = os.path.join( os.path.normpath(avr_gcc_root), os.path.normpath('/bin/avr-g++' ) )
cxx     = avr_gcc_root + os.path.normpath('/bin/avr-g++')
objcopy = avr_gcc_root + os.path.normpath('/bin/avr-objcopy')
avrsize = avr_gcc_root + os.path.normpath('/bin/avr-size')
avrdude = avr_gcc_root + os.path.normpath('/bin/avrdude')

def run( cmd ):
    print( cmd )
    os.system( cmd )

def upload_hex( filename, mcu ):
    #print( "Upload '{filename}'".format(filename=filename) )
    cmd = cmd_upl.format( filename=filename, mcu=mcu, avrdude=avrdude,  )
    run( cmd )


if __name__ == '__main__':
    if len( sys.argv ) > 1:
        upload_hex( sys.argv[1], mcu )
    else:
        print( 'Usage: {} file.hex'.format( sys.argv[0] ) )

# end of file
