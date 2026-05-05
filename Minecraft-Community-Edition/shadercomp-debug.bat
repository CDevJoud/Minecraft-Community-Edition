@echo off
setlocal enabledelayedexpansion

REM Root paths
set ROOT=%cd%
REM Right now the only way to get the shader compiler is to build it from src from bgfx repo
set SC=D:\Void\bgfx\bin\shadercRelease
set INCLUDE=%ROOT%\libs\bgfx\
set SHADERS=%ROOT%\Graphics\Shaders
set OUT=%ROOT%\assets\shaders
goto :gen

REM ===== FUNCTION =====
REM %1 = platform
REM %2 = renderer name (d3d11, gl, etc)
REM %3 = profile
:compile
echo Compiling %2 for %1...

%SC% -f %SHADERS%\main.vs -o %OUT%\main.vs.%2_%1.bin --type v --platform %1 -p %3 -i %INCLUDE%
%SC% -f %SHADERS%\main.fs -o %OUT%\main.fs.%2_%1.bin --type f --platform %1 -p %3 -i %INCLUDE%

goto :eof

:gen
REM ===== WINDOWS =====
call :compile windows d3d11 s_5_0
call :compile windows d3d12 s_5_0
call :compile windows gl 120
call :compile windows vulkan spirv10-10

REM ===== LINUX =====
call :compile linux gl 120
call :compile linux vulkan spirv10-10

REM ===== MACOS =====
call :compile osx metal metal
call :compile osx gl 120

REM ===== IOS =====
call :compile osx metal metal
call :compile osx gl 120

REM ===== ANDROID =====
call :compile android gl 120
call :compile android vulkan spirv10-10

REM ===== asm.js =====
call :compile asm.js gl 120

echo Done!
