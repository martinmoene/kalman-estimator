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
import glob
import os
import re
import sys

# Configuration:

# atmega328: avr5 - "Enhanced" devices with 16 KiB up to 64 KiB of program memory.

def_opt  = '2'
def_std  = 'c++17'
def_mcu  = 'atmega328p'
def_fcpu = '16000000'

# End configuration

# AVR-GCC programs:

avr_gcc_root = os.environ['AVR_GCC_ROOT']

if len( avr_gcc_root ) == 0:
    print("Expected environment variable 'AVR_GCC_ROOT' to be set to root of AVR-GCC.")
    sys.exit()

#cpp     = os.path.join( os.path.normpath(avr_gcc_root), os.path.normpath('/bin/avr-g++' ) )
cxx     = avr_gcc_root + os.path.normpath('/bin/avr-g++')
objcopy = avr_gcc_root + os.path.normpath('/bin/avr-objcopy')
avrsize = avr_gcc_root + os.path.normpath('/bin/avr-size')
avrdude = avr_gcc_root + os.path.normpath('/bin/avrdude')

# Derived configuration:

cxxflags_asm = '-Wall -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD'
cxxflags_obj = '{cxxflags_asm} -flto'.format(cxxflags_asm=cxxflags_asm)

lflags  = '-flto -fuse-linker-plugin -Wl,--gc-sections'

cmd_asm = '"{cxx}" -std={std} -O{opt} {cxxflags} -mmcu={mcu} -DAVR -DF_CPU_HZ={fcpu} {defines} {includes} -S -o {outname}.s {inpname}'
cmd_obj = '"{cxx}" -std={std} -O{opt} {cxxflags} -mmcu={mcu} -DAVR -DF_CPU_HZ={fcpu} {defines} {includes} -c -o {outname}.o {inpname}'

cmd_elf = '"{cxx}" -O{opt} {lflags} -mmcu={mcu} -o {outname}.elf {outname}.o -lm'
cmd_eep = '"{objcopy}" -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 {outname}.elf {outname}.eep'
cmd_hex = '"{objcopy}" -O ihex -R .eeprom {outname}.elf {outname}.hex'
cmd_size= '"{avrsize}" {outname}.hex'

# End configuration

def run(opt, cmd):
    if opt.debug > 1:
        print(cmd)
    os.system(cmd)

def expand_list(prefix, lst):
    return ' '.join([prefix + s for s in lst])

def compile_to_asm(opt, inp_file, out_base):
    """Compile to assembly discarding warnings and errors:"""
    if opt.verbose > 2:
        print('- Compile to assembly: {i} => {o}:'.format(i=inp_file, o=out_base))
    cmd = cmd_asm.format(
        inpname=inp_file, outname=out_base,
        cxx=cxx, std=opt.std, opt=opt.O,
        cxxflags=cxxflags_asm, mcu=opt.mmcu, fcpu=opt.fcpu,
        defines=expand_list('-D', opt.D), includes=expand_list('-I', opt.I))
    run(opt, cmd)

def compile_to_obj(opt, inp_file, out_base):
    """Compile to object code, displaying warnings and errors:"""
    if opt.verbose > 2:
        print('- Compile to object code: {i} => {o}:'.format(i=inp_file, o=out_base))
    cmd = cmd_obj.format(
        inpname=inp_file, outname=out_base,
        cxx=cxx, std=opt.std, opt=opt.O,
        cxxflags=cxxflags_obj, mcu=opt.mmcu, fcpu=opt.fcpu,
        defines=expand_list('-D', opt.D), includes=expand_list('-I', opt.I))
    run(opt, cmd)

def link_to_elf(opt, out_base):
    """Link to ELF:"""
    if opt.verbose > 2:
        print('- Link to ELF: {f}.o => {f}.elf:'.format(f=out_base))
    cmd = cmd_elf.format(
        outname=out_base,
        cxx=cxx, opt=opt.O,
        lflags=lflags, mcu=opt.mmcu)
    run(opt, cmd)

def create_eep(opt, out_base):
    """Create EEP:"""
    if opt.verbose > 2:
        print('- Create .eep: {f}.elf => {f}.eep:'.format(f=out_base))
    cmd = cmd_eep.format(
        outname=out_base,
        objcopy=objcopy)
    run(opt, cmd)

def create_hex(opt, out_base):
    """Create HEX:"""
    if opt.verbose > 2:
        print('- Create .hex: {f}.elf => {f}.hex:'.format(f=out_base))
    cmd = cmd_hex.format(
        outname=out_base,
        objcopy=objcopy)
    run(opt, cmd)

def print_size(opt, out_base):
    """Print code and data sizes:"""
    if opt.verbose > 2:
        print('- Print code and data sizes from {f}.hex:'.format(f=out_base) )
    cmd = cmd_size.format(
        outname=out_base,
        avrsize=avrsize)
    if opt.verbose > 1:
        run(opt, cmd)

def buildSingle(opt, inp_file, out_base):
    """Build compilation and link products for single file"""
    if opt.verbose:
        print( ('AVG-GCC {i} => {o} .s/.o/.elf/.eep/.hex:' if out_base else '{i}:').format(i=inp_file, o=out_base) )
    compile_to_asm(opt, inp_file, out_base)
    compile_to_obj(opt, inp_file, out_base)
    link_to_elf(   opt, out_base)
    create_eep(    opt, out_base)
    create_hex(    opt, out_base)
    print_size(    opt, out_base)
    return 1

def build(opt, paths, out_base):
    """Build compilations and link products"""
    n = 0
    for g in paths:
        for f in glob.glob(g):
            if os.path.isfile(f):
                if not out_base:
                    out_base = os.path.splitext(f)[0]
                n = n + buildSingle(opt, inp_file=f, out_base=out_base)
    return n

def file_count(paths):
    n = 0
    for g in paths:
        for f in glob.glob(g):
            if os.path.isfile(f):
                n += 1
    return n

def main():
    """Compile with avr-gcc."""

    parser = argparse.ArgumentParser(
        description='Compile with avr-gcc and create .s/.o/.elf/.eep/.hex files.',
        epilog="""""",
        formatter_class=argparse.RawTextHelpFormatter)

    parser.add_argument(
        'Input',
        metavar='file',
        type=str,
        nargs=argparse.REMAINDER,
        help='files to process')

    parser.add_argument(
        '-d', '--debug',
        action='count',
        help='report debug information (2 levels)')

    parser.add_argument(
        '-v', '--verbose',
        action='count',
        help='report processing steps (3 levels)')

    parser.add_argument(
        '-D',
        metavar='<definition>',
        action='append',
        default=[],
        type=str,
        help='Preprocessor definition')

    parser.add_argument(
        '-I',
        metavar='<path>',
        action='append',
        default=[],
        type=str,
        help='Preprocessor inclusion path')

    parser.add_argument(
        '-O',
        metavar='<optimization>',
        type=str,
        default=def_opt,
        help='Optimization level [-O{}]'.format(def_opt))

    parser.add_argument(
        '-std',
        metavar='<standard>',
        type=str,
        default=def_std,
        help='C++ standard [{}]'.format(def_std))

    parser.add_argument(
        '-mmcu',
        metavar='<mcu>',
        type=str,
        default=def_mcu,
        help='AVR architecture [{}]'.format(def_mcu))

    parser.add_argument(
        '-fcpu',
        metavar='<F>',
        type=str,
        default=def_fcpu,
        help='Processor clock frequency in Hz [{}]'.format(def_fcpu))

    parser.add_argument(
        '--output',
        metavar='output',
        type=str,
        help='basename of output files [basename of input file]')

    opt = parser.parse_args()

    if opt.debug > 0:
        print('Environment: avr_gcc_root={avr_gcc_root}'.format(avr_gcc_root=avr_gcc_root))
        print('Program: {}'.format(cxx))
        print('Program: {}'.format(objcopy))
        print('Program: {}'.format(avrsize))
        print('Options: {}'.format(opt))

    if opt.output and file_count(opt.Input) > 1:
        raise RuntimeError("Cannot use option '--output {o}' with multiple input files.".format(o=args.output))

    count = build(opt, paths=opt.Input, out_base=opt.output)

    if count > 0:
        print( "Processed " + str(count) + " file" + ("(s)" if count > 1 else "") )
    else:
        print( "No files compiled" )


if __name__ == '__main__':
    try:
        main()
    except Exception as e:
        print('{p}: Error: {e}'.format(p=os.path.basename(sys.argv[0]), e=e) )

# end of file
