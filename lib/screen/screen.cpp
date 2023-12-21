#include "screen.h"

void Screen::drawGraphics(Chip8 chip8)
{
    printf("Drawing graphics\n");
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
    {
        int x = (i % SCREEN_WIDTH) * 20 + 10;
        int y = -(i / SCREEN_WIDTH) * 20 + 640;

        if (chip8.gfx[i] == 1){}
            // dc.SetPen(wxPen(wxColor(0, 0, 0), 10));

        if (chip8.gfx[i] == 0){}
            // dc.SetPen(wxPen(wxColor(255, 255, 255), 10));

        // dc.DrawRectangle(x, y, 10, 10);
    }
}

void Screen::setupGraphics()
{
    printf("Setting up graphics\n");
}

void Screen::setupInput()
{
    printf("Setting up input\n");
}