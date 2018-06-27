@echo off & setlocal enableextensions enabledelayedexpansion
::
:: tc.bat - compile & run tests (clang).
::

:: if no std is given, use c++11

set std=%1
set args=%2 %3 %4 %5 %6 %7 %8 %9
if "%1" == "" set std=c++17

set clang=C:\Program Files\LLVM\bin\clang

::call :CompilerVersion version
::echo clang %version%: %std% %args%

set ke_feature=^
    -Dke_FEATURE_XXX=1

set lest_defines=^
    -Dlest_FEATURE_AUTO_REGISTER

set flags=-Wpedantic -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-missing-noreturn -Wno-documentation-unknown-command -Wno-documentation-deprecated-sync -Wno-documentation -Wno-weak-vtables -Wno-missing-prototypes -Wno-missing-variable-declarations -Wno-exit-time-destructors -Wno-global-constructors

set ke_program=main.t.exe
set ke_sources=main.t.cpp stdcpp.t.cpp fixed-point.t.cpp matrix.t.cpp

"%clang%" -std=%std% -O2 -Wall -Wextra %flags% %ke_feature%  %lest_defines% -fms-compatibility-version=19.00 -isystem "%VCInstallDir%include" -isystem "%WindowsSdkDir_71A%include" -I../include -o %ke_program% %ke_sources% && %ke_program%

endlocal & goto :EOF

:: subroutines:

:CompilerVersion  version
echo off & setlocal enableextensions
set tmpprogram=_getcompilerversion.tmp
set tmpsource=%tmpprogram%.c

echo #include ^<stdio.h^>     > %tmpsource%
echo int main(){printf("%%d.%%d.%%d\n",__clang_major__,__clang_minor__,__clang_patchlevel__);} >> %tmpsource%

"%clang%" -o %tmpprogram% %tmpsource%
::>nul
for /f %%x in ('%tmpprogram%') do set version=%%x
::del %tmpprogram%.* >nul
endlocal & set %1=%version%& goto :EOF
