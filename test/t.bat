@echo off & setlocal enableextensions enabledelayedexpansion
::
:: t.bat - compile & run tests (MSVC).
::

set std=%1
if not "%std%"=="" (
    set std=-std:%std%
) else (
    set std=-std:c++latest
)

set CppCoreCheckInclude=%VCINSTALLDIR%\Auxiliary\VS\include

call :CompilerVersion version
echo VC%version%: %args%

set ke_feature=^
    -Dke_FEATURE_XXX=1

set lest_defines=^
    -Dlest_FEATURE_AUTO_REGISTER

set msvc_defines=^
    -DNOMINMAX ^
    -D_CRT_SECURE_NO_WARNINGS ^
    -D_SCL_SECURE_NO_WARNINGS

set ke_program=main.t.exe
set ke_sources=main.t.cpp fixed-point.t.cpp matrix.t.cpp

cl -W3 -EHsc %std% %ke_feature% %lest_defines% %msvc_defines% -I../include %ke_sources% && %ke_program%
endlocal & goto :EOF

:: subroutines:

:CompilerVersion  version
@echo off & setlocal enableextensions
set tmpprogram=_getcompilerversion.tmp
set tmpsource=%tmpprogram%.c

echo #include ^<stdio.h^>                   >%tmpsource%
echo int main(){printf("%%d\n",_MSC_VER);} >>%tmpsource%

cl /nologo %tmpsource% >nul
for /f %%x in ('%tmpprogram%') do set version=%%x
del %tmpprogram%.* >nul
set offset=0
if %version% LSS 1900 set /a offset=1
set /a version="version / 10 - 10 * ( 5 + offset )"
endlocal & set %1=%version%& goto :EOF
