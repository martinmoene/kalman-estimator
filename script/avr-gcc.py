#!/usr/bin/env python

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
import platform
import re
import sys

# Configuration:

# atmega328: avr5 - "Enhanced" devices with 16 KiB up to 64 KiB of program memory.

def_opt  = '2'
def_std  = 'c++17'
def_mcu  = 'atmega328p'
def_fcpu = '16000000'

# End configuration

def initialize():

    try:
        global avr_gcc_root; avr_gcc_root = os.environ['AVR_GCC_ROOT']
    except KeyError as e:
        raise RuntimeError("expected environment variable {} to be set to root of AVR-GCC.".format(e))

    # AVR-GCC programs:

    #cpp     = os.path.join( os.path.normpath(avr_gcc_root), os.path.normpath('/bin/avr-g++' ) )
    global cxx;     cxx     = avr_gcc_root + os.path.normpath('/bin/avr-g++')
    global objcopy; objcopy = avr_gcc_root + os.path.normpath('/bin/avr-objcopy')
    global avrsize; avrsize = avr_gcc_root + os.path.normpath('/bin/avr-size')
    global avrdude; avrdude = avr_gcc_root + os.path.normpath('/bin/avrdude')

    # Derived configuration:

    global cxxflags_asm; cxxflags_asm = '-Wall -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD'
    global cxxflags_obj; cxxflags_obj = '{cxxflags_asm} -flto'.format(cxxflags_asm=cxxflags_asm)

    global lflags;   lflags  = '-flto -fuse-linker-plugin -Wl,--gc-sections'

    global cmd_mac;  cmd_mac = '"{cxx}" -std={std} -O{opt} -mmcu={mcu} -xc++ -E -dM - < {nul}'
    global cmd_asm;  cmd_asm = '"{cxx}" -std={std} -O{opt} {cxxflags} -mmcu={mcu} -DF_CPU_HZ={fcpu} {defines} {includes} -S -o {outname}.s {inpname}'
    global cmd_obj;  cmd_obj = '"{cxx}" -std={std} -O{opt} {cxxflags} -mmcu={mcu} -DF_CPU_HZ={fcpu} {defines} {includes} -c -o {outname}.o {inpname}'

    global cmd_elf;  cmd_elf = '"{cxx}" -O{opt} {lflags} -mmcu={mcu} -o {outname}.elf {files} -lm'
    global cmd_eep;  cmd_eep = '"{objcopy}" -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 {outname}.elf {outname}.eep'
    global cmd_hex;  cmd_hex = '"{objcopy}" -O ihex -R .eeprom {outname}.elf {outname}.hex'
    global cmd_size; cmd_size= '"{avrsize}" {outname}.hex'

def run(opt, cmd):
    if opt.debug > 1:
        print(cmd)
    os.system(cmd)

def remove_ext(path):
    return os.path.splitext(path)[0]

def to_obj(paths):
    return [remove_ext(f) + '.o' for f in paths]

def expand_list(prefix, lst):
    return ' '.join([prefix + s for s in lst])

def dev_null():
    return 'nul' if platform.system()=='Windows' else '/dev/null'

def report_macros(opt):
    """List built-in macros:"""
    if opt.verbose > 2:
        print('List predefined macros:')
    cmd = cmd_mac.format(
        cxx=cxx, std=opt.std, opt=opt.O, mcu=opt.mmcu, nul=dev_null())
    run(opt, cmd)

def compile_to_asm(opt, inp_file, out_base):
    """Compile to assembly discarding warnings and errors:"""
    if opt.verbose > 2:
        print('- Compile to assembly: {i} => {o}.s:'.format(i=inp_file, o=out_base))
    cmd = cmd_asm.format(
        inpname=inp_file, outname=out_base,
        cxx=cxx, std=opt.std, opt=opt.O,
        cxxflags=cxxflags_asm, mcu=opt.mmcu, fcpu=opt.fcpu,
        defines=expand_list('-D', opt.D), includes=expand_list('-I', opt.I))
    run(opt, cmd)

def compile_to_obj(opt, inp_file, out_base):
    """Compile to object code, displaying warnings and errors:"""
    if opt.verbose > 2:
        print('- Compile to object code: {i} => {o}.o:'.format(i=inp_file, o=out_base))
    cmd = cmd_obj.format(
        inpname=inp_file, outname=out_base,
        cxx=cxx, std=opt.std, opt=opt.O,
        cxxflags=cxxflags_obj, mcu=opt.mmcu, fcpu=opt.fcpu,
        defines=expand_list('-D', opt.D), includes=expand_list('-I', opt.I))
    run(opt, cmd)

def compile_cpp(opt, inp_file, out_base):
    if opt.verbose > 0:
        print('{}:'.format(inp_file))
    compile_to_asm(opt, inp_file, out_base)
    compile_to_obj(opt, inp_file, out_base)
    return 1

def link_to_elf(opt, out_base, obj_files):
    """Link to ELF:"""
    if opt.verbose > 2:
        print('- Link to ELF: {f} => {o}.elf:'.format(f=obj_files, o=out_base))
    cmd = cmd_elf.format(
        outname=out_base, files=' '.join(obj_files),
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

def build(opt, paths, out_base):
    """Build compilations and link products"""
    n = 0
    if not out_base:
        out_base = remove_ext(paths[0])
    for f in paths:
        n = n + compile_cpp(opt, inp_file=f, out_base=remove_ext(f))
    link_to_elf(   opt, out_base, to_obj(paths) )
    create_eep(    opt, out_base)
    create_hex(    opt, out_base)
    print_size(    opt, out_base)
    return n

def file_list(paths):
    result = []
    for g in paths:
        result.extend( [f for f in glob.glob(g) if os.path.isfile(f)])
    return result

def main():
    """Compile with avr-gcc."""

    initialize()

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
        '-m', '--macros',
        action='count',
        help='report predefined macros')

    parser.add_argument(
        '-D',
        metavar='<definition>',
        action='append',
        default=[],
        type=str,
        help='preprocessor definition')

    parser.add_argument(
        '-I',
        metavar='<path>',
        action='append',
        default=[],
        type=str,
        help='preprocessor inclusion path')

    parser.add_argument(
        '-O',
        metavar='<optimization>',
        type=str,
        default=def_opt,
        help='optimization level [-O{}]'.format(def_opt))

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
        help='processor clock frequency in Hz [{}]'.format(def_fcpu))

    parser.add_argument(
        '--output',
        metavar='output',
        type=str,
        help='basename of output files [basename of input file]')

    opt = parser.parse_args()

    if opt.debug > 0:
        print('OS: {}, {}, {}'.format(os.name, platform.system(), platform.release()))
        print('Environment: avr_gcc_root={avr_gcc_root}'.format(avr_gcc_root=avr_gcc_root))
        print('Program: {}'.format(cxx))
        print('Program: {}'.format(objcopy))
        print('Program: {}'.format(avrsize))
        print('Options: {}'.format(opt))

    if opt.macros:
        return report_macros(opt)

    count = build(opt, paths=file_list(opt.Input), out_base=opt.output)

    if count > 0:
        print( '{}: processed {} file{}'.format(parser.prog, str(count), 's' if count > 1 else ''))
    else:
        print('{}: no files compiled'.format(parser.prog))


if __name__ == '__main__':
    try:
        main()
    except Exception as e:
        print('{p}: error: {e}'.format(p=os.path.basename(sys.argv[0]), e=e) )

# end of file
