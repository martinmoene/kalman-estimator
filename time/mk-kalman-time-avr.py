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

std      = 'c++17'
mcu      = 'atmega328p'
fcpu     = '16000000'
basename = 'kalman-time-avr'

# Derived configuration:

cxxflags_asm = "-Wall -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -mmcu={mcu} -DAVR -DF_CPU_HZ={fcpu}".format( mcu=mcu, fcpu=fcpu )
cxxflags_obj = "{cxxflags_asm} -flto".format( cxxflags_asm=cxxflags_asm )

lflags       = "-flto -fuse-linker-plugin -Wl,--gc-sections -mmcu={mcu}".format( mcu=mcu )


#cmd_cxx = '"{cxx}" -std={std} -{opt} {cxxflags}  -DKE_NUMERIC_TYPE={num_type} -I{include}'
cmd_asm = '"{cxx}" -std={std} -{opt} {cxxflags} -DKE_NUMERIC_TYPE={num_type} -DKE_UPDATE_KALMAN_GAIN={kalman_gain} -I{include} -S -o {outname}.s {basename}.cpp'
cmd_obj = '"{cxx}" -std={std} -{opt} {cxxflags} -DKE_NUMERIC_TYPE={num_type} -DKE_UPDATE_KALMAN_GAIN={kalman_gain} -I{include} -c -o {outname}.o {basename}.cpp'

cmd_elf = '"{cxx}" -{opt} {lflags} -o {outname}.elf {outname}.o -lm'
cmd_eep = '"{objcopy}" -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 {outname}.elf {outname}.eep'
cmd_hex = '"{objcopy}" -O ihex -R .eeprom {outname}.elf {outname}.hex'
cmd_size= '"{avrsize}" {outname}.hex'

cmd_upl = '{avrdude} -v -p{mcu} -cusbtiny -Uflash:w:{outname}.hex:i'

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
    #print( cmd )
    os.system( cmd )

def compile_to_asm( basename, outname, num_type, kalman_gain, cxx, std, opt, cxxflags, include ):
    """Compile to assembly discarding warnings and errors:"""
    #print( "Compile to '{outname}.s'".format(outname=outname) )
    cmd = cmd_asm.format(
            basename=basename, outname=outname, num_type=num_type, kalman_gain=kalman_gain, cxx=cxx, std=std, opt=opt, cxxflags=cxxflags, include=include )
    run( cmd )

def compile_to_obj( basename, outname, num_type, kalman_gain, cxx, std, opt, cxxflags, include ):
    """Compile to object code, displaying warnings and errors:"""
    #print( "Compile to '{outname}.o'".format(outname=outname) )
    cmd = cmd_obj.format(
            basename=basename, outname=outname, num_type=num_type, kalman_gain=kalman_gain, cxx=cxx, std=std, opt=opt, cxxflags=cxxflags, include=include )
    run( cmd )

def link_to_elf( outname, opt, lflags, cxx ):
    """Link to ELF:"""
    #print( "Link to '{outname}.elf'".format(outname=outname) )
    cmd = cmd_elf.format(
            outname=outname, opt=opt, lflags=lflags, cxx=cxx )
    run( cmd )

def create_eep( outname, objcopy ):
    """Create EEP:"""
    #print( "Create '{outname}.eep'".format(outname=outname) )
    cmd = cmd_eep.format( outname=outname, objcopy=objcopy )
    run( cmd )

def create_hex( outname, objcopy ):
    """Create HEX:"""
    #print( "Create '{outname}.hex'".format(outname=outname) )
    cmd = cmd_hex.format( outname=outname, objcopy=objcopy )
    run( cmd )

def print_size( outname, avrsize ):
    #print( "Determine size from '{outname}.hex'".format(outname=outname) )
    cmd = cmd_size.format( outname=outname, avrsize=avrsize )
    run( cmd )

def upload_hex( outname, mcu ):
    #print( "Upload '{outname}.hex'".format(outname=outname) )
    cmd = cmd_upl.format( outname=outname, mcu=mcu )
    run( cmd )

def build( basename ):
    kg_text = ['fixed', 'updat']

    for num_type in ['double', 'fp32_t']:
        for kalman_gain in [1, 0]:
            for optimization in ['Os', 'O2']:
                outname = '{basename}-{nt}-{kg}-{opt}'.format( basename=basename, nt=num_type, kg=kg_text[kalman_gain], opt=optimization )
                print('\nCompiling: {basename}.cpp to {outname}.hex'.format( basename=basename, outname=outname ) )
                compile_to_asm( basename, outname, num_type=num_type, kalman_gain=kalman_gain, cxx=cxx, std=std, opt=optimization, cxxflags=cxxflags_asm, include='../include/' )
                compile_to_obj( basename, outname, num_type=num_type, kalman_gain=kalman_gain, cxx=cxx, std=std, opt=optimization, cxxflags=cxxflags_obj, include='../include/' )
                link_to_elf(    outname, opt=optimization, lflags=lflags, cxx=cxx )
                create_eep(     outname, objcopy=objcopy )
                create_hex(     outname, objcopy=objcopy )
                print_size(     outname, avrsize=avrsize )
#                upload_hex(     outname, avrdude=avrdude, mcu=mcu )

if __name__ == '__main__':
    build( 'kalman-time-avr' )

# end of file
