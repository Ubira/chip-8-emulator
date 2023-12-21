# Compile
g++ main.cpp ../lib/chip-8/chip-8.cpp ../lib/screen/screen.cpp -std=c++14 -I../lib/chip-8 -I../lib/screen -lSDL2 -o chip8 -Wall

