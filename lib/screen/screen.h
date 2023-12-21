#ifndef SCREEN_H
#define SCREEN_H

#include <cstdio>
#include <iostream>
#include <unistd.h>
#include "chip-8.h"

class Screen
{
public:
    void setupGraphics();
    void setupInput();
    void drawGraphics(Chip8 chip8);
};

#endif /* SCREEN_H */