#!/usr/bin/env python

#
# Copyright 2018 by Martin Moene
#
# https://github.com/martinmoene/kalman-estimator
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

from __future__ import print_function

import os
import sys

def main():
    try:
        global avr_gcc_root; avr_gcc_root = os.environ['AVR_GCC_ROOT']
    except KeyError as e:
        raise RuntimeError("expected environment variable {} to be set to root of AVR-GCC.".format(e))

    cxx = avr_gcc_root + os.path.normpath('/bin/avr-g++')
    cmd = '"{cxx}" -E -dM - < nul'.format(cxx=cxx)

    os.system( cmd)

if __name__ == '__main__':
    try:
        main()
    except Exception as e:
        print('{p}: error: {e}'.format(p=os.path.basename(sys.argv[0]), e=e) )

# end of file
