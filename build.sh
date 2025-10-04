
cp -r assets build
gcc src/*.c glad/src/gl.c -o build/ldjam -lSDL3 -lGL -Iglad/include -lm -g
