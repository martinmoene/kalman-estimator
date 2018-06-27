@echo off & setlocal enableextensions enabledelayedexpansion
::
:: avr.bat - run avr-g++(AVR-GCC).
::

if [%AVR_GCC_ROOT%] == [] goto NO_VAR

set cpp=%AVR_GCC_ROOT%\bin\avr-g++

"%cpp%" %*

endlocal & goto :EOF

:NO_VAR
echo %0: Missing environment variable AVR_GCC_ROOT.
