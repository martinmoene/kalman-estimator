#!/usr/bin/env python

# Copyright 2018 by Martin Moene
#
# https://github.com/martinmoene/kalman-estimator
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

import os

msvc = 1

nt       = 'double'
nt       = 'fp32_t'

std      = 'c++17'
opt      = '-O2'
basename = 'kalman-sim'
cppname  = basename + '.cpp'
exename  = basename + '.exe'
output   = basename + '.out.txt'

if msvc:
    tpl = 'cl -EHsc -std:c++latest {opt} -DKE_NUMERIC_TYPE={nt} -I../include {cppname} && {exename}'
else:
    tpl = 'g++ -std={std} -Wall {opt} -DKE_NUMERIC_TYPE={nt} -I../include -o {exename} {cppname} && {exename}'

cmd = tpl.format(nt=nt, std=std, opt=opt, cppname=cppname, exename=exename)

print(     cmd )
os.system( cmd )
