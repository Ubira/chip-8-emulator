#include "chip-8.h"

void Chip8::initialize()
{
    printf("Initializing emulator...\n");
    program_counter = 0x200; // Program counter starts at 0x200
    opcode_ = 0;             // Reset current opcode
    index_register = 0;      // Reset index register
    stack_pointer = 0;       // Reset stack pointer
    draw_flag = false;

    // Clear display
    printf("Clearing display...\n");
    std::memset(&gfx, 0, (SCREEN_WIDTH * SCREEN_HEIGHT) * sizeof(uint8_t));

    // Clear stack
    printf("Clearing stack...\n");
    std::memset(&stack, 0, STACK_SIZE * sizeof(uint16_t));

    // Clear registers V0-VF
    printf("Clearing General Purpose Registers...\n");
    for (int i = 0; i < GPREG_NUM; i++)
    {
        std::memset(&gen_purpose_reg_v[i], 0, sizeof(uint8_t));
    }

    // Clear memory
    printf("Cleaing memory...\n");
    std::memset(&memory_, 0, MEM_SIZE * sizeof(u_int8_t));

    // Load fontset
    printf("Loading font set...\n");
    for (int i = 0; i < FONT_SET_SIZE; ++i)
        memory_[i] = chip8_fontset[i];

    // Reset timers
    printf("Resetting timers...\n");
    delay_timer = 0;
    sound_timer = 0;
}

void Chip8::emulateCycle()
{
    printf("Fetching and Decoding OPCODE... ");
    // Fetch opcode
    opcode_ = memory_[program_counter] << 8 | memory_[program_counter + 1];

    printf("Fetched opcode: 0x%X\n", opcode_);
    printf("program_counter: 0x%X\n", program_counter);

    // Decode opcode
    switch (opcode_ & 0xF000)
    {

        // case 0x0000: // 0x0NNN: Calls machine code routine (RCA 1802 for COSMAC VIP) at address NNN. Not necessary for most ROMs
        // TODO UJUN: Implement OPCODE
        // printf("Unknown opcode [0x0000]: 0x%X\n", opcode_);
        // break;

    case 0x0000:
        switch (opcode_ & 0x0FFF)
        {
        case 0x00E0: // 0x00E0: Clears the screen
            for (u_int i = 0; i < sizeof(gfx); i++)
            {
                gfx[i] = 0;
            }
            draw_flag = true;
            program_counter += 2;
            break;

        case 0x00EE: // 0x00EE: Returns from subroutine
            program_counter = stack[stack_pointer - 1];
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
        if (gen_purpose_reg_v[opcode_ & 0x0F00 >> 8] == (opcode_ & 0xFF))
            program_counter += 4;
        else
            program_counter += 2;
        break;

    case 0x4000: // 4XNN: Skips the next instruction if VX does not equal NN (usually the next instruction is a jump to skip a code block)
        if (gen_purpose_reg_v[opcode_ & 0x0F00 >> 8] != (opcode_ & 0xFF))
            program_counter += 4;
        else
            program_counter += 2;
        break;

    case 0x5000: // 5XY0: Skips the next instruction if VX equals VY (usually the next instruction is a jump to skip a code block)
        if (gen_purpose_reg_v[opcode_ & 0x0F00 >> 8] == gen_purpose_reg_v[opcode_ & 0x00F0 >> 4])
            program_counter += 4;
        else
            program_counter += 2;
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
            program_counter += 2;
            break;

        case 0x0002: // 8XY2: Sets VX to VX and VY. (bitwise AND operation)
            gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8] &= gen_purpose_reg_v[(opcode_ & 0x00F0) >> 4];
            program_counter += 2;
            break;

        case 0x0003: // 8XY3: Sets VX to VX xor VY.
            gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8] ^= gen_purpose_reg_v[(opcode_ & 0x00F0) >> 4];
            program_counter += 2;
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
            gen_purpose_reg_v[0xF] = 1;
            for (u_int i = 0; i < sizeof(gen_purpose_reg_v[0]); i++)
            {
                if (!(gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8]) && (gen_purpose_reg_v[(opcode_ & 0x00F0) >> 4]))
                {
                    gen_purpose_reg_v[0xF] = 0;
                }
            }
            gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8] -= gen_purpose_reg_v[(opcode_ & 0x00F0) >> 4];
            program_counter += 2;
            break;

        case 0x0006: // 8XY6: Stores the least significant bit of VX in VF and then shifts VX to the right by 1
            gen_purpose_reg_v[(opcode_ & 0x00F0) >> 4] = gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8] & 0x1;
            gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8] >>= 1;
            program_counter += 2;
            break;

        case 0x0007: // 8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there is not
            gen_purpose_reg_v[0xF] = 1;
            for (u_int i = 0; i < sizeof(gen_purpose_reg_v[0]); i++)
            {
                if ((gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8]) && !(gen_purpose_reg_v[(opcode_ & 0x00F0) >> 4]))
                {
                    gen_purpose_reg_v[0xF] = 0;
                }
            }
            gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8] = gen_purpose_reg_v[(opcode_ & 0x00F0) >> 4] - gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8];
            program_counter += 2;
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

    case 0x9000: // 9XY0: Skips the next instruction if VX does not equal VY. (Usually the next instruction is a jump to skip a code block)
        if (gen_purpose_reg_v[opcode_ & 0x0F00 >> 8] != gen_purpose_reg_v[opcode_ & 0x00F0 >> 4])
            program_counter += 4;
        else
            program_counter += 2;
        break;

    case 0xA000: // ANNN: Sets I to the address NNN
        index_register = opcode_ & 0x0FFF;
        program_counter += 2;
        break;

    case 0xB000: // BNNN: Jumps to the address NNN plus V0
        program_counter = (opcode_ & 0xFFF) + gen_purpose_reg_v[0];
        break;

    case 0xC000: // CXNN: Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN
    {
        srand(time(NULL));
        uint8_t _rand_ = rand() % 255;
        gen_purpose_reg_v[(opcode_ & 0xF00) >> 8] = _rand_ & (opcode_ & 0xFF);
        program_counter += 2;
    }
        break;

    case 0xD000: // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
                 // Each row of 8 pixels is read as bit-coded starting from memory location I; I value does not change after the execution of this instruction.
                 // As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that does not happen
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
        draw_flag = true;
        program_counter += 2;
    }
    break;

    case 0xE000:
        switch (opcode_ & 0x00FF)
        {
        case 0x009E: // EX9E: Skips the next instruction if the key stored in VX is pressed (usually the next instruction is a jump to skip a code block)
            if (key[gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8]] != 0)
                program_counter += 4;
            else
                program_counter += 2;
            break;

        case 0x00A1: // EXA1: Skips the next instruction if the key stored in VX is not pressed (usually the next instruction is a jump to skip a code block)
            if (key[gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8]] == 0)
                program_counter += 4;
            else
                program_counter += 2;
            break;

        default:
            printf("Unknown opcode [0xE000]: 0x%X\n", opcode_);
            break;
        }
        break;

    case 0xF000:
        switch (opcode_ & 0x00FF)
        {
        case 0x0007: // FX07: Sets VX to the value of the delay timer
            gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8] = delay_timer;
            program_counter += 2;
            break;

        case 0x000A: // FX0A: A key press is awaited, and then stored in VX (blocking operation, all instruction halted until next key event)
            char _char_;
            std::cin >> _char_;
            gen_purpose_reg_v[(opcode_ & 0xF00) >> 8] = (uint8_t)_char_;
            program_counter += 2;
            break;

        case 0x0015: // FX15: Sets the delay timer to VX
            delay_timer = gen_purpose_reg_v[(opcode_ & 0xF00) >> 8];
            program_counter += 2;
            break;

        case 0x0018: // FX18: Sets the sound timer to VX
            sound_timer = gen_purpose_reg_v[(opcode_ & 0xF00) >> 8];
            program_counter += 2;
            break;

        case 0x001E: // FX1E: Adds VX to I. VF is not affected
            index_register += gen_purpose_reg_v[(opcode_ & 0xF00) >> 8];
            program_counter += 2;
            break;

        case 0x0029: // FX29: Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font
            index_register = chip8_fontset[gen_purpose_reg_v[(opcode_ & 0xF00) >> 8]];
            program_counter += 2;
            break;

        case 0x0033: // FX33: Stores the binary-coded decimal representation of VX, with the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2
            memory_[index_register] = gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8] / 100;
            memory_[index_register + 1] = (gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8] / 10) % 10;
            memory_[index_register + 2] = (gen_purpose_reg_v[(opcode_ & 0x0F00) >> 8] % 100) % 10;
            program_counter += 2;
            break;

        case 0x0055: // FX55: Stores from V0 to VX (including VX) in memory, starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified
            for (int i = 0; i <= ((opcode_ & 0xF00) >> 8); i++)
            {
                memory_[index_register + i] = gen_purpose_reg_v[i];
            }
            program_counter += 2;
            break;

        case 0x0065: // FX65: Fills from V0 to VX (including VX) with values from memory, starting at address I. The offset from I is increased by 1 for each value read, but I itself is left unmodified
            for (int i = 0; i <= ((opcode_ & 0xF00) >> 8); i++)
            {
                gen_purpose_reg_v[i] = memory_[index_register + i];
            }
            program_counter += 2;
            break;

        default:
            printf("Unknown opcode [0xFX00]: 0x%X\n", opcode_);
            break;
        }
        break;

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

void Chip8::loadGame(char *game_name)
{
    // Use fopen (in binary mode) and start filling the memory at location: 0x200 == 512
    printf("Loading game into memory...\n");
    FILE *fp = std::fopen(game_name, "r+b");
    if (!fp)
    {
        std::perror("File opening failed");
    }
    int c;
    int i{0};
    while ((c = std::fgetc(fp)) != EOF) // standard C I/O file reading loop
    {
        memory_[i + 512] = (uint8_t)c;
        printf("memory_[%d]: %x\n", i + 512, memory_[i + 512]);
        i++;
    }
    fclose(fp);
    printf("Game Loaded!\n");
}

void Chip8::setKeys()
{
    printf("Fetching pressed key...\n");
    std::memset(&key, 0, KEY_NUM);

    char _char_;
    std::cin >> _char_;
    switch (_char_)
    {
    case '1':
        key[0] = 1;
        break;

    case '2':
        key[1] = 1;
        break;

    case '3':
        key[2] = 1;
        break;

    case '4':
        key[3] = 1;
        break;

    case 'q':
        key[4] = 1;
        break;

    case 'w':
        key[5] = 1;
        break;

    case 'e':
        key[6] = 1;
        break;

    case 'r':
        key[7] = 1;
        break;

    case 'a':
        key[8] = 1;
        break;

    case 's':
        key[9] = 1;
        break;

    case 'd':
        key[10] = 1;
        break;

    case 'f':
        key[11] = 1;
        break;

    case 'z':
        key[12] = 1;
        break;

    case 'x':
        key[13] = 1;
        break;

    case 'c':
        key[14] = 1;
        break;

    case 'v':
        key[15] = 1;
        break;

    default:
        printf("Unknown key: %c\n", _char_);
        break;
    }
}

/*
OPCODES

0NNN	Call		Calls machine code routine (RCA 1802 for COSMAC VIP) at address NNN. Not necessary for most ROMs.
00E0	Display	disp_clear()	Clears the screen.
00EE	Flow	return;	Returns from a subroutine.
1NNN	Flow	goto NNN;	Jumps to address NNN.
2NNN	Flow	*(0xNNN)()	Calls subroutine at NNN.
3XNN	Cond	if (Vx == NN)	Skips the next instruction if VX equals NN (usually the next instruction is a jump to skip a code block).
4XNN	Cond	if (Vx != NN)	Skips the next instruction if VX does not equal NN (usually the next instruction is a jump to skip a code block).
5XY0	Cond	if (Vx == Vy)	Skips the next instruction if VX equals VY (usually the next instruction is a jump to skip a code block).
6XNN	Const	Vx = NN	Sets VX to NN.
7XNN	Const	Vx += NN	Adds NN to VX (carry flag is not changed).
8XY0	Assig	Vx = Vy	Sets VX to the value of VY.
8XY1	BitOp	Vx |= Vy	Sets VX to VX or VY. (bitwise OR operation)
8XY2	BitOp	Vx &= Vy	Sets VX to VX and VY. (bitwise AND operation)
8XY3[a]	BitOp	Vx ^= Vy	Sets VX to VX xor VY.
8XY4	Math	Vx += Vy	Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there is not.
8XY5	Math	Vx -= Vy	VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there is not.
8XY6[a]	BitOp	Vx >>= 1	Stores the least significant bit of VX in VF and then shifts VX to the right by 1.[b]
8XY7[a]	Math	Vx = Vy - Vx	Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there is not.
8XYE[a]	BitOp	Vx <<= 1	Stores the most significant bit of VX in VF and then shifts VX to the left by 1.[b]
9XY0	Cond	if (Vx != Vy)	Skips the next instruction if VX does not equal VY. (Usually the next instruction is a jump to skip a code block);
ANNN	MEM	I = NNN	Sets I to the address NNN.
BNNN	Flow	PC = V0 + NNN	Jumps to the address NNN plus V0.
CXNN	Rand	Vx = rand() & NN	Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
DXYN	Display	draw(Vx, Vy, N)	Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value does not change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that does not happen.
EX9E	KeyOp	if (key() == Vx)	Skips the next instruction if the key stored in VX is pressed (usually the next instruction is a jump to skip a code block).
EXA1	KeyOp	if (key() != Vx)	Skips the next instruction if the key stored in VX is not pressed (usually the next instruction is a jump to skip a code block).
FX07	Timer	Vx = get_delay()	Sets VX to the value of the delay timer.
FX0A	KeyOp	Vx = get_key()	A key press is awaited, and then stored in VX (blocking operation, all instruction halted until next key event).
FX15	Timer	delay_timer(Vx)	Sets the delay timer to VX.
FX18	Sound	sound_timer(Vx)	Sets the sound timer to VX.
FX1E	MEM	I += Vx	Adds VX to I. VF is not affected.[c]
FX29	MEM	I = sprite_addr[Vx]	Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
FX33	BCD
            set_BCD(Vx)
            *(I+0) = BCD(3);
            *(I+1) = BCD(2);
            *(I+2) = BCD(1);
                                Stores the binary-coded decimal representation of VX, with the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.
FX55	MEM	reg_dump(Vx, &I)	Stores from V0 to VX (including VX) in memory, starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.[d]
FX65	MEM	reg_load(Vx, &I)	Fills from V0 to VX (including VX) with values from memory, starting at address I. The offset from I is increased by 1 for each value read, but I itself is left unmodified.[d]

*/