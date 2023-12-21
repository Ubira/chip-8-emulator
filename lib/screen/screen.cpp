#include "screen.h"

void Screen::drawGraphics(SDL_Renderer* renderer, SDL_Rect* rect, Chip8 chip8)
{
    printf("Drawing graphics\n");
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
    {
        int x = (i % SCREEN_WIDTH) * 20 + 0;
        int y = (i / SCREEN_WIDTH) * 20 + 0;

        // SDL_RenderClear(renderer);

        if (chip8.gfx[i] == 1)
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            // dc.SetPen(wxPen(wxColor(0, 0, 0), 10));

        if (chip8.gfx[i] == 0)
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            // dc.SetPen(wxPen(wxColor(255, 255, 255), 10));

        rect->x = x;
        rect->y = y;
        SDL_RenderFillRect(renderer, rect);
        // dc.DrawRectangle(x, y, 10, 10);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
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