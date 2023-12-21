#include "screen.h" // OpenGL graphics and input
#include "chip-8.h" // Your cpu core implementation

#include <SDL2/SDL.h>

constexpr int LOOP_DURATION = 1000 / 60; // 60 FPS 


Screen myScreen;
Chip8 myChip8;

int main(int argc, char **argv)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
		std::cout << "Error SDL2 Initialization : " << SDL_GetError();
		return 1;
	}

    SDL_Window* window = SDL_CreateWindow("First program", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	if (window == NULL) {
		std::cout << "Error window creation";
		return 3;
	}

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		std::cout << "Error renderer creation";
		return 4;
	}

    // Set up render system and register input callbacks
    myScreen.setupGraphics();
    myScreen.setupInput();

    // Initialize the Chip8 system and load the game into the memory
    myChip8.initialize();     // Clear the memory, registers and screen
    myChip8.loadGame("../roms/pong.ch8"); // Copy the program into the memory

    SDL_Event e;
    u_int32_t start_tick;
    
    // Emulation loop
    for (;;)
    {
        start_tick = SDL_GetTicks();

        // Emulate one cycle
        myChip8.emulateCycle();

        // If the draw flag is set, update the screen
        if (myChip8.draw_flag) // Only two opcodes should set this flag: 0x00E0 (Clears the screen) and 0xDXYN (Draws a sprite on the screen)
        {
            myScreen.drawGraphics(myChip8);
            myChip8.draw_flag = false;
        }

        // Store key press state (Press and Release)
        // myChip8.setKeys();

	    if (SDL_PollEvent(&e)) {
		    if (e.type == SDL_QUIT) {
			    break;
		    }
	    }
        SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);

        // Ensure loop runs for at least a certain duration
        if (SDL_GetTicks() - start_tick < LOOP_DURATION) {
            SDL_Delay(LOOP_DURATION - (SDL_GetTicks() - start_tick));
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
	SDL_Quit();

    return 0;
}