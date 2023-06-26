#include "chip-8.h"

void Chip8::initialize()
{
    program_counter = 0x200; // Program counter starts at 0x200
    opcode_ = 0;             // Reset current opcode
    index_register = 0;      // Reset index register
    stack_pointer = 0;       // Reset stack pointer

    // TODO UJUN: Clear display
    // TODO UJUN: Clear stack
    // TODO UJUN: Clear registers V0-VF
    // TODO UJUN: Clear memory

    // Load fontset
    for (int i = 0; i < 80; ++i)
        memory_[i] = chip8_fontset[i];

    // TODO UJUN: Reset timers
}

void Chip8::emulateCycle()
{
    // Fetch opcode
    opcode_ = memory_[program_counter] << 8 | memory_[program_counter + 1];

    // Decode opcode
    switch (opcode_ & 0xF000)
    {

    case 0x0000:
        switch (opcode_ & 0x000F)
        {
        case 0x0000: // 0x00E0: Clears the screen
                     // Execute opcode
            break;

        case 0x000E: // 0x00EE: Returns from subroutine
            program_counter = stack[stack_pointer];
            stack_pointer--;
            program_counter += 2;
            break;

        default:
            printf("Unknown opcode [0x0000]: 0x%X\n", opcode_);
            break;
        }
        break;

    case 0x1000: // 1NNN: Jumps to address NNN
        program_counter = opcode_ & 0xFFF;
        break;

    case 0x2000: // 2NNN: Calls subroutine at NNN
        stack[stack_pointer] = program_counter;
        stack_pointer++;
        program_counter = opcode_ & 0xFFF;
        break;

    case 0x3000: // 3XNN: Skips the next instruction if VX equals NN (usually the next instruction is a jump to skip a code block)
        if (gen_purpose_reg_v[opcode_ & 0x0F00 >> 8] == opcode_ & 0xFF)
            program_counter += 4;
        break;

    case 0x4000: // 4XNN: Skips the next instruction if VX does not equal NN (usually the next instruction is a jump to skip a code block)
        if (gen_purpose_reg_v[opcode_ & 0x0F00 >> 8] != opcode_ & 0xFF)
            program_counter += 4;
        break;

    case 0x5000: // 5XY0: Skips the next instruction if VX equals VY (usually the next instruction is a jump to skip a code block)
        if (gen_purpose_reg_v[opcode_ & 0x0F00 >> 8] == gen_purpose_reg_v[opcode_ & 0x00F0 >> 4])
            program_counter += 4;
        break;

    case 0x6000: // 6XNN: Sets VX to NN
        gen_purpose_reg_v[opcode_ & 0xF00 >> 8] = opcode_ & 0xFF;
        program_counter += 2;
        break;

    case 0x7000: // 7XNN: Adds NN to VX (carry flag is not changed)
        gen_purpose_reg_v[opcode_ & 0xF00 >> 8] += opcode_ & 0xFF;
        program_counter += 2;
        break;

    case 0x8000:
        switch (opcode_ & 0xF)
        {
        case 0x0000: // 8XY0: Sets VX to the value of VY
            gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8] = gen_purpose_reg_v[(opcode_ & 0x00F0) >> 4];
            program_counter += 2;
            break;

        case 0x0001: // 8XY1: Sets VX to VX or VY. (bitwise OR operation)
            gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8] |= gen_purpose_reg_v[(opcode_ & 0x00F0) >> 4];
            break;

        case 0x0002: // 8XY2: Sets VX to VX and VY. (bitwise AND operation)
            gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8] &= gen_purpose_reg_v[(opcode_ & 0x00F0) >> 4];
            break;

        case 0x0003: // 8XY3: Sets VX to VX xor VY.
            gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8] ^= gen_purpose_reg_v[(opcode_ & 0x00F0) >> 4];
            break;

        case 0x0004: // 8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there is not
            if ((gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8] + gen_purpose_reg_v[(opcode_ & 0x00F0) >> 4]) >> 8)
                gen_purpose_reg_v[16] = 1;
            else
                gen_purpose_reg_v[16] = 0;
            gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8] += gen_purpose_reg_v[(opcode_ & 0x00F0) >> 4];
            program_counter += 2;
            break;

        case 0x0005: // 8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there is not
            // TODO UJUN: Execute opcode
            break;

        case 0x0006: // 8XY6: Stores the least significant bit of VX in VF and then shifts VX to the right by 1
            gen_purpose_reg_v[(opcode_ & 0x00F0) >> 4] = gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8] & 0x1;
            gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8] >>= 1;
            break;

        case 0x0007: // 8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there is not
            // TODO UJUN: Execute opcode
            break;

        case 0x000E: // 89XYE:  Stores the most significant bit of VX in VF and then shifts VX to the left by 1
            gen_purpose_reg_v[(opcode_ & 0x00F0) >> 4] = gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8] & 0x80;
            gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8] <<= 1;
            break;

        default:
            printf("Unknown opcode [0x8XY0]: 0x%X\n", opcode_);
            break;
        }
        break;

    case 0xA000: // ANNN: Sets I to the address NNN
        // TODO UJUN: Execute opcode
        index_register = opcode_ & 0x0FFF;
        program_counter += 2;
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

        // TODO UJUN: More opcodes //

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
    // TODO UJUN: Use fopen in binary mode) and start filling the memory at location: 0x200 == 512
    for (int i = 0; i < bufferSize; ++i)
        memory_[i + 512] = buffer[i];
}