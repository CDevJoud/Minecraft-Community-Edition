@echo off
setlocal enabledelayedexpansion

REM Root paths
set ROOT=%cd%
REM Right now the only way to get the shader compiler is to build it from src from bgfx repo
set SC=D:\Void\bgfx\bin\shadercRelease
set INCLUDE=%ROOT%\libs\bgfx\
set SHADERS=%ROOT%\Graphics\Shaders
set RMLSHADERS=%ROOT%\UI\Backend\shaders
set OUT=%ROOT%\assets\shaders
goto :gen

REM ===== FUNCTION =====
REM %1 = platform
REM %2 = renderer name (d3d11, gl, etc)
REM %3 = profile
:compile
echo Compiling %2 for %1...

REM Graphics default shader
%SC% -f %SHADERS%\main.vs -o %OUT%\main.vs.%2_%1.bin --type v --platform %1 -p %3 -i %INCLUDE%
%SC% -f %SHADERS%\main.fs -o %OUT%\main.fs.%2_%1.bin --type f --platform %1 -p %3 -i %INCLUDE%

REM RmlUI shaders
%SC% -f %RMLSHADERS%\rmlui.vs -o %OUT%\rmlui.vs.%2_%1.bin --type v --platform %1 -p %3 %INCLUDE%
%SC% -f %RMLSHADERS%\rmlui_passthrough.vs -o %OUT%\rmlui_passthrough.vs.%2_%1.bin --type v --platform %1 -p %3 %INCLUDE%
%SC% -f %RMLSHADERS%\rmlui_blendmask.fs -o %OUT%\rmlui_blendmask.fs.%2_%1.bin --type f --platform %1 -p %3 %INCLUDE%
%SC% -f %RMLSHADERS%\rmlui_blur.fs -o %OUT%\rmlui_blur.fs.%2_%1.bin --type f --platform %1 -p %3 %INCLUDE%
%SC% -f %RMLSHADERS%\rmlui_color.fs -o %OUT%\rmlui_color.fs.%2_%1.bin --type f --platform %1 -p %3 %INCLUDE%
%SC% -f %RMLSHADERS%\rmlui_colormatrix.fs -o %OUT%\rmlui_colormatrix.fs.%2_%1.bin --type f --platform %1 -p %3 %INCLUDE%
%SC% -f %RMLSHADERS%\rmlui_creation.fs -o %OUT%\rmlui_creation.fs.%2_%1.bin --type f --platform %1 -p %3 %INCLUDE%
%SC% -f %RMLSHADERS%\rmlui_dropshadow.fs -o %OUT%\rmlui_dropshadow.fs.%2_%1.bin --type f --platform %1 -p %3 %INCLUDE%
%SC% -f %RMLSHADERS%\rmlui_gradient.fs -o %OUT%\rmlui_gradient.fs.%2_%1.bin --type f --platform %1 -p %3 %INCLUDE%
%SC% -f %RMLSHADERS%\rmlui_passthrough.fs -o %OUT%\rmlui_passthrough.fs.%2_%1.bin --type f --platform %1 -p %3 %INCLUDE%
%SC% -f %RMLSHADERS%\rmlui_texture.fs -o %OUT%\rmlui_texture.fs.%2_%1.bin --type f --platform %1 -p %3 %INCLUDE%

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
