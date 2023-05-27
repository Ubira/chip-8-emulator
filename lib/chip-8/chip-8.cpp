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
    opcode_ = memory_[program_counter] << 8 | memory_[program_counter + 1];

    // Decode opcode
    switch (opcode_ & 0xF000)
    {
    case 0xA000: // ANNN: Sets I to the address NNN
        // Execute opcode
        index_register = opcode_ & 0x0FFF;
        program_counter += 2;
        break;

    case 0x2000: // 2NNN: Calls subroutine at NNN
        stack[stack_pointer] = program_counter;
        stack_pointer++;
        program_counter = opcode_ & 0xFFF;
        break;

    case 0x8000:
        switch (opcode_ & 0xF)
        {
        case 0x0004: // 8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there is not
            if ((gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8] + gen_purpose_reg_v[(opcode_ & 0x00F0) >> 4]) >> 8)
                gen_purpose_reg_v[16] = 1;
            else
                gen_purpose_reg_v[16] = 0;
            gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8] += gen_purpose_reg_v[(opcode_ & 0x00F0) >> 4];
            program_counter += 2;
            break;

        default:
            printf("Unknown opcode [0x8XY0]: 0x%X\n", opcode_);
            break;
        }
        break;

    case 0xD000:
    {
        uint8_t x = gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8];
        uint8_t y = gen_purpose_reg_v[(opcode_ & 0x00F0) >> 4];
        uint8_t height = opcode_ & 0x000F;
        uint8_t pixel;

        gen_purpose_reg_v[0xF] = 0;

        for (int yline = 0; yline < height; yline++)
        {
            pixel = memory_[index_register + yline];
            for (int xline = 0; xline < 8; xline++)
            {
                if ((pixel & (0x80 >> xline)) != 0)
                {
                    if (gfx[(x + xline + ((y + yline) * 64))] == 1)
                        gen_purpose_reg_v[0xF] = 1;
                    gfx[x + xline + ((y + yline) * 64)] ^= 1;
                }
            }
        }
        drawFlag = true;
        program_counter += 2;
    }
    break;

    case 0xE000:
        switch (opcode_ & 0x00FF)
        {
        // EX9E: Skips the next instruction
        // if the key stored in VX is pressed
        case 0x009E:
            if (key[gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8]] != 0)
                program_counter += 4;
            else
                program_counter += 2;
            break;

        default:
            printf("Unknown opcode [0xE000]: 0x%X\n", opcode_);
            break;
        }

    case 0xF000:
        switch (opcode_ & 0x00FF)
        {
        case 0x0033:
            memory_[index_register] = gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8] / 100;
            memory_[index_register + 1] = (gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8] / 10) % 10;
            memory_[index_register + 2] = (gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8] % 100) % 10;
            program_counter += 2;
            break;

        default:
            printf("Unknown opcode [0xFX00]: 0x%X\n", opcode_);
            break;
        }
        break;

    case 0x0000:
        switch (opcode_ & 0x000F)
        {
        case 0x0000: // 0x00E0: Clears the screen
                     // Execute opcode
            break;

        case 0x000E: // 0x00EE: Returns from subroutine
                     // Execute opcode
            break;

        default:
            printf("Unknown opcode [0x0000]: 0x%X\n", opcode_);
            break;
        }
        break;
        // More opcodes //

    default:
        printf("Unknown opcode: 0x%X\n", opcode_);
        break;
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
    for (int i = 0; i < bufferSize; ++i)
        memory_[i + 512] = buffer[i];
}