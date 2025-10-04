@echo off
setlocal
set PATH=.\w64devkit\bin;%PATH%

set SDL3_DIR=sdl3/SDL3-3.2.24/x86_64-w64-mingw32

gcc src/main.c src/render.c src/entity.c src/audio.c glad/src/gl.c -o build/ldjam.exe -lSDL3 -mwindows -Iglad/include -L%SDL3_DIR%/lib -I%SDL3_DIR%/include -I.
