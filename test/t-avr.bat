@echo off & setlocal enableextensions enabledelayedexpansion
::
:: t-avr.bat - compile & run tests (AVR-GCC).
::

:: atmega328: avr5 - "Enhanced" devices with 16 KiB up to 64 KiB of program memory.

set opt=
if [%1] == [] set opt=-O2

set KE_NUMERIC_TYPE=double
::set KE_NUMERIC_TYPE=fp32_t

call %~dp0..\script\avr.bat -std=c++17 -Wall -I../include %opt% %* -mmcu=avr5 -S -DAVR=1 -DKE_NUMERIC_TYPE=%KE_NUMERIC_TYPE% main-avr.t.cpp

endlocal & goto :EOF
