#include "screen.h" // OpenGL graphics and input
#include "chip-8.h" // Your cpu core implementation

Screen myScreen;
Chip8 myChip8;

int main(int argc, char **argv)
{
    // Set up render system and register input callbacks
    myScreen.setupGraphics();
    myScreen.setupInput();

    // Initialize the Chip8 system and load the game into the memory
    myChip8.initialize();     // Clear the memory, registers and screen
    myChip8.loadGame("../roms/pong.ch8"); // Copy the program into the memory

    // Emulation loop
    for (;;)
    {
        // Emulate one cycle
        myChip8.emulateCycle();

        // If the draw flag is set, update the screen
        if (myChip8.draw_flag) // Only two opcodes should set this flag: 0x00E0 (Clears the screen) and 0xDXYN (Draws a sprite on the screen)
        {
            myScreen.drawGraphics();
            myChip8.draw_flag = false;
        }

        // Store key press state (Press and Release)
        myChip8.setKeys();
    }

    return 0;
}