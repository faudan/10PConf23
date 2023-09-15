@echo off
@REM vcvars32 for 32bits, vcvars64 for 64bits
set compilerPath="C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
where /q cl
if errorlevel 1 call %compilerPath% > NUL

if not exist "build" mkdir "build"

set fileName="%~dp0/src\win32_main.cpp"
set "optimization=/O2"
@REM -GR- turn off runtime type info
@REM -Gm- turn off incremental build
@REM -Oi makes the compiler use compiler intrinsics (prefer assemebly version of code)
@REM -Od disables compiler optimizations
@REM -Z7 generates debug information, simpler format
@REM -EHa- disables exception handling
@REM -MD searches dll, -MT uses static linking
@REM -Fm<filename> creates a .map file that lists all the functions
set "flags=-nologo -Gm- -GR- -Od -Oi -Z7 -EHa- -MT -Fmhandmade"

@REM -wd.... disables warning code
@REM -WX.... warning = error
@REM -w4.... only consider warnings from own code
set "warnings= -W4 -wd4211 -wd4201 -wd4100 -wd4189 -wd4505"

@REM -D..... sets env vars
set "buildVars=-DHANDMADE_SLOW=0 -DHANDMADE_INTERNAL=0"

@REM -subsystem:windows for windows, -subsystem:console for console programs
@REM -opt:ref removes things that are not used
@REM winmm.lib to set scheduler granularity
set "linkers=/link -opt:ref -subsystem:windows,5.1 user32.lib gdi32.lib winmm.lib" 

pushd "build"
@echo on
cl  %warnings% %flags% %buildVars% %fileName% %linkers%
@echo off
popd 
exit