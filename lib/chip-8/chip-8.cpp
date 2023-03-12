#include "chip-8.h"

void Chip8::initialize()
{
    program_counter = 0x200; // Program counter starts at 0x200
    opcode_ = 0;             // Reset current opcode
    index_register = 0;      // Reset index register
    stack_pointer = 0;       // Reset stack pointer

    // Clear display
    // Clear stack
    // Clear registers V0-VF
    // Clear memory

    // Load fontset
    for (int i = 0; i < 80; ++i)
        memory_[i] = chip8_fontset[i];

    // Reset timers
}

void Chip8::emulateCycle()
{
    // Fetch opcode
    opcode = memory[pc] << 8 | memory[pc + 1];

    // Decode opcode
    switch (opcode & 0xF000)
    {
        // Some opcodes //

    case 0xA000: // ANNN: Sets I to the address NNN
        // Execute opcode
        I = opcode & 0x0FFF;
        pc += 2;
        break;

        // More opcodes //

    default:
        printf("Unknown opcode: 0x%X\n", opcode);
    }

    // Update timers
    if (delay_timer > 0)
        --delay_timer;

    if (sound_timer > 0)
    {
        if (sound_timer == 1)
            printf("BEEP!\n");
        --sound_timer;
    }
}

void Chip8::loadGame(char16_t game_name)
{
    // Use fopen in binary mode) and start filling the memory at location: 0x200 == 512
}