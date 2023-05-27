#ifndef CHIP_8_H
#define CHIP_8_H

#include <cstdint>
#include <cstdio>

class Chip8
{
    uint16_t opcode_;
    uint8_t memory_[4096];
    uint8_t gen_purpose_reg_v[16]; // General purpose registers V0, V1, ..., VE + VF (overflow register)

    uint16_t index_register;  // Can have value from 0x000 to 0xFFF
    uint16_t program_counter; // Can have value from 0x000 to 0xFFF

    /*
        *** System memory map ***
        0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
        0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
        0x200-0xFFF - Program ROM and work RAM
    */

    uint8_t gfx[64 * 32]; // 64 x 32 = 2048 pixels screen
    uint8_t delay_timer;
    uint8_t sound_timer;

    uint16_t stack[16];
    uint16_t stack_pointer;

    uint8_t key[16]; // HEX based keypad (0x0-0xF)
                     //
                     // Keypad                   Keyboard
                     // +-+-+-+-+                +-+-+-+-+
                     // |1|2|3|C|                |1|2|3|4|
                     // +-+-+-+-+                +-+-+-+-+
                     // |4|5|6|D|                |Q|W|E|R|
                     // +-+-+-+-+       =>       +-+-+-+-+
                     // |7|8|9|E|                |A|S|D|F|
                     // +-+-+-+-+                +-+-+-+-+
                     // |A|0|B|F|                |Z|X|C|V|
                     // +-+-+-+-+                +-+-+-+-+

    uint8_t chip8_fontset[80] =
        {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    bool drawFlag;

public:
    void initialize();
    void emulateCycle();
    void loadGame(char16_t game_name);
};

#endif /* CHIP_8_H */