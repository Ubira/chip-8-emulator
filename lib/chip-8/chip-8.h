#ifndef CHIP_8_H
#define CHIP_8_H

#include <cstdint>
#include <cstdio>

class Chip8
{
    uint16_t opcode_;
    uint8_t memory_[4096];
    uint8_t gen_purpose_reg_v[16]; // General purpose registers V0, V1, ..., VE

    uint16_t index_register;  // Can have value from 0x000 to 0xFFF
    uint16_t program_counter; // Can have value from 0x000 to 0xFFF

    /*
        0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
        0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
        0x200-0xFFF - Program ROM and work RAM
    */

    uint8_t gfx[64 * 32];
    uint8_t delay_timer;
    uint8_t sound_timer;

    uint16_t stack[16];
    uint16_t stack_pointer;

    uint8_t key[16]; // HEX based keypad (0x0-0xF)

public:
    void initialize();
    void emulateCycle();
    void loadGame(char16_t game_name);
};

#endif /* CHIP_8_H */