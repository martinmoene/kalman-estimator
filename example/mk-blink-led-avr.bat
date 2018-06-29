@echo off & setlocal enableextensions enabledelayedexpansion
::
:: mk-blink-avr.bat - compile, link program and create hex file (AVR-GCC).
::

:: atmega328: avr5 - "Enhanced" devices with 16 KiB up to 64 KiB of program memory.
set std=c++17
set mcu=atmega328p
set fcpu=16000000L
set basename=blink-led-avr

if [%AVR_GCC_ROOT%] == [] goto NO_VAR

set     cpp=%AVR_GCC_ROOT%\bin\avr-g++
set objcopy=%AVR_GCC_ROOT%\bin\avr-objcopy
set avrsize=%AVR_GCC_ROOT%\bin\avr-size
set avrdude=%AVR_GCC_ROOT%\bin\avrdude

set opt=
if [%1] == [] set opt=-Os

:: Compile flags common to assembly and object code:
:: -fpermissive
set cppflags1=^
 -Wall^
 -fno-exceptions ^
 -ffunction-sections ^
 -fdata-sections ^
 -fno-threadsafe-statics ^
 -MMD ^
 -mmcu=%mcu% ^
 -DF_CPU_HZ=%fcpu%

:: Add compile flags for object code:
set cppflags2=^
 %cppflags1% ^
 -flto

 :: Link flags:
set lflags=^
 -flto ^
 -fuse-linker-plugin ^
 -Wl,--gc-sections ^
 -mmcu=%mcu%

::echo on

:: Compile to assembly discarding warnings and errors:
echo Compile to %basename%.s
"%cpp%" -std=%std% %opt% %* %cppflags1% -I../include -S %basename%.cpp > nul 2>&1

:: Compile to object code, displaying warnings and errors:
echo Compile to %basename%.o
"%cpp%" -std=%std% %opt% %* %cppflags2% -I../include -o %basename%.o -c %basename%.cpp

:: Link to ELF:
echo Link to %basename%.elf
"%cpp%" %opt% %lflags% -o %basename%.elf %basename%.o -lm

:: Create EEP:
echo Create %basename%.eep
"%objcopy%" -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 %basename%.elf %basename%.eep

:: Create HEX:
echo Create %basename%.hex
"%objcopy%" -O ihex -R .eeprom %basename%.elf %basename%.hex

"%avrsize%" %basename%.hex

:: Upload hex file
::%avrdude% -v -patmega328p -cusbtiny -Uflash:w:%basename%.hex:i

endlocal & goto :EOF

:NO_VAR
echo %0: Missing environment variable AVR_GCC_ROOT.
