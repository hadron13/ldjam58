@echo off
setlocal
set PATH=.\w64devkit\bin;%PATH%

set SDL3_DIR=SDL3-3.2.24/x86_64-w64-mingw32

gcc src/main.c glad/src/gl.c -o build/ldjam.exe  -lSDL3 -mwindows -Iglad/include -LSDL3_DIR/lib -ISDL3_DIR/include
