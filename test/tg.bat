@echo off & setlocal enableextensions enabledelayedexpansion
::
:: tg.bat - compile & run tests (GNUC).
::

:: if no std is given, use c++11

set std=%1
set args=%2 %3 %4 %5 %6 %7 %8 %9
if "%1" == "" set std=c++17

call :CompilerVersion version
echo g++ %version%: %std% %args%

set ke_feature=^
    -DKE_USE_STATIC_EXPECT=1

set lest_defines=^
    -Dlest_FEATURE_AUTO_REGISTER

set flags=-Wpedantic -Wno-padded -Wno-missing-noreturn
set   gpp=g++

set ke_program=main.t.exe
set ke_sources=main.t.cpp stdcpp.t.cpp fixed-point.t.cpp matrix.t.cpp

%gpp% -std=%std% -O2 -Wall -Wextra %flags% %ke_feature% %lest_defines% -o %ke_program% -I../include %ke_sources% && %ke_program%

endlocal & goto :EOF

:: subroutines:

:CompilerVersion  version
echo off & setlocal enableextensions
set tmpprogram=_getcompilerversion.tmp
set tmpsource=%tmpprogram%.c

echo #include ^<stdio.h^>     > %tmpsource%
echo int main(){printf("%%d.%%d.%%d\n",__GNUC__,__GNUC_MINOR__,__GNUC_PATCHLEVEL__);} >> %tmpsource%

g++ -o %tmpprogram% %tmpsource% >nul
for /f %%x in ('%tmpprogram%') do set version=%%x
del %tmpprogram%.* >nul
endlocal & set %1=%version%& goto :EOF
