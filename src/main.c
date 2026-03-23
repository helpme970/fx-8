#include <gint/display.h>
#include <gint/keyboard.h>
#include <stdint.h>
#include <gint/std/stdio.h>
#include <gint/std/stdlib.h>
#include <gint/std/string.h>
// #include <gint/rtc.h>
#include <gint/timer.h>
#include <gint/clock.h>

// easier use of gint's bfile library
#include "file2.h"
// #include "menu.h"
// #include "menu.c"

#define START_ADDRESS 0x200
#define FONTSET_SIZE 16*5
#define FONTSET_START_ADDRESS 0x50

// CHIP-8 hardware
static uint8_t *memory = NULL;
uint8_t  display[64 * 32];
uint8_t  V[16];  // Registers
uint16_t I;
uint16_t pc;
uint8_t delayTimer;
uint8_t soundTimer;
uint16_t stack[16];
uint16_t sp;
uint8_t  keyboard[16];
uint16_t opcode;
uint8_t  drawFlag;

// functions
void initChip8();
void loadROM(const char* filename);
void draw();
void execute();
void print(char* text);

void main() {
    dclear(C_WHITE);
	memory = calloc(4096, sizeof(uint8_t));
	if (memory == NULL) {
		print("Error");
        exit(1);
	}

	initChip8();
    loadROM("Breakout.ch8");

	uint64_t speed =  (1 * 1000 * 1000) / 60;

    uint8_t running = 1;
    key_event_t key;

    // char romn[40];
	// int chpos = 0;
    // chpos = menu_filechooser("*.ch8","Choose ROM",(char*)&romn,chpos);
    // if (chpos == -1) exit(1);

    while (running) {
		key = pollevent();

        switch (key.type) {
            case KEYEV_DOWN:
            // case KEYEV_HOLD:
                switch (key.key) {
                    case KEY_0:
                        keyboard[0xA] = 1;
                        break;
                    case KEY_1:
                        keyboard[0x7] = 1;
                        break;
                    case KEY_2:
                        keyboard[0x8] = 1;
                        break;
                    case KEY_3:
                        keyboard[0x9] = 1;
                        break;
                    case KEY_4:
                        keyboard[0x4] = 1;
                        break;
                    case KEY_5:
                        keyboard[0x5] = 1;
                        break;
                    case KEY_6:
                        keyboard[0x6] = 1;
                        break;
                    case KEY_7:
                        keyboard[0x1] = 1;
                        break;
                    case KEY_8:
                        keyboard[0x2] = 1;
                        break;
                    case KEY_9:
                        keyboard[0x3] = 1;
                        break;
                    case KEY_DOT:
                        keyboard[0x0] = 1;
                        break;
                    case KEY_EXP:
                        keyboard[0xB] = 1;
                        break;
                    case KEY_DEL:
                        keyboard[0xC] = 1;
                        break;
                    case KEY_MUL:
                        keyboard[0xD] = 1;
                        break;
                    case KEY_ADD:
                        keyboard[0xE] = 1;
                        break;
                    case KEY_NEG:
                        keyboard[0xF] = 1;
                }
                break;
            case KEYEV_UP:
                switch (key.key) {
                    case KEY_EXE:
                        running = 0;
                        break;
                        break;
                    case KEY_0:
                        keyboard[0xA] = 0;
                        break;
                    case KEY_1:
                        keyboard[0x7] = 0;
                        break;
                    case KEY_2:
                        keyboard[0x8] = 0;
                        break;
                    case KEY_3:
                        keyboard[0x9] = 0;
                        break;
                    case KEY_4:
                        keyboard[0x4] = 0;
                        break;
                    case KEY_5:
                        keyboard[0x5] = 0;
                        break;
                    case KEY_6:
                        keyboard[0x6] = 0;
                        break;
                    case KEY_7:
                        keyboard[0x1] = 0;
                        break;
                    case KEY_8:
                        keyboard[0x2] = 0;
                        break;
                    case KEY_9:
                        keyboard[0x3] = 0;
                        break;
                    case KEY_DOT:
                        keyboard[0x0] = 0;
                        break;
                    case KEY_EXP:
                        keyboard[0xB] = 0;
                        break;
                    case KEY_DEL:
                        keyboard[0xC] = 0;
                        break;
                    case KEY_MUL:
                        keyboard[0xD] = 0;
                        break;
                    case KEY_ADD:
                        keyboard[0xE] = 0;
                        break;
                    case KEY_NEG:
                        keyboard[0xF] = 0;
                        break;
                }
                break;
        }

        if (speed > 0) {
            sleep_us(speed); // delay in microseconds; 1 sec = 1 000 000 microsec
        }

        if (delayTimer > 0) {
            delayTimer--;
        }

        execute();
        if(drawFlag) {
            draw();
            drawFlag = 0;
        }
	}
	free(memory);
}

inline void initChip8() {
	const uint8_t fonts[FONTSET_SIZE] = {
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
    memset(display, 0, 64 * 32);
    memset(V, 0, 16);
    I = 0;
    pc = START_ADDRESS;
    delayTimer = 0;
    soundTimer = 0;
    memset(stack, 0, 16);
    sp = 0;
    memset(keyboard, 0, 16);
    memcpy(memory + FONTSET_START_ADDRESS, fonts, 80 * sizeof(uint8_t));
}

inline void loadROM(const char* filepath) {
    {
    int infile = open(filepath, "r");
    if (infile < 0) {
        print("Couldn't open ROM");
		exit(1);
    }

    int size = BFile_Size(infile);

    BFile_Read(infile, memory + 0x200, size, -1);
    
    BFile_Close(infile);
    }
}

inline void draw() {
    int x, y;

    dclear(C_WHITE);
    for (x = 0; x < 64; x++) {
        for (y = 0; y < 32; y++) {
            if (display[x + (y * 64)] == 1) {
                dpixel(x*2, y*2, C_BLACK);
                dpixel(x*2+1, y*2, C_BLACK);
                dpixel(x*2, y*2+1, C_BLACK);
                dpixel(x*2+1, y*2+1, C_BLACK);
            }
        }
    }
    dupdate();
}

inline void execute() {
    uint8_t  X, Y, kk, n;
    uint16_t nnn;

    // Fetch
    opcode = memory[pc] << 8 | memory[pc + 1];
    pc += 2;

    // Decoding
    X = (opcode & 0x0F00) >> 8;
    Y = (opcode & 0x00F0) >> 4;
    n = (opcode & 0x000F);
    kk = (opcode & 0x00FF);
    nnn = (opcode & 0x0FFF);

    // char text[4];
    // sprintf(text, "%x", opcode);
    // print(text);
    // print("Opcode: %x\n", opcode);
    // printf("Program Counter: %x\n", pc);
    // printf("I: %x\n", I);

    switch (opcode & 0xF000)
    {
        case 0x0000:
            switch (opcode & 0x00FF)
            {
                // 00E0
                case 0x00E0:
                    memset(display, 0, 64 * 32);
                    drawFlag = 1;
                    break;
                // 00EE
                case 0x00EE:
                    sp--;
                    pc = stack[sp];
                    break;
                default:
                    printf("Error executing opcode %x\n", opcode);
                    break;
            }
            break;
        // 1NNN
        case 0x1000:
            pc = nnn;
            break;
        // 2NNN
        case 0x2000:
            stack[sp] = pc;
            sp++;
            pc = nnn;
            break;
        // 3XNN
        case 0x3000:
            if (V[X] == kk)
            {
                pc += 2;
            }
            break;
        // $XNN
        case 0x4000:
            if (V[X] != kk)
            {
                pc += 2;
            }
            break;
        // 5XY0
        case 0x5000:
            if (V[X] == V[Y])
            {
                pc += 2;
            }
            break;
        // 6XNN
        case 0x6000:
            V[X] = kk;
            break;
        // 7XNN
        case 0x7000:
            V[X] += kk;
            break;
        case 0x8000:
            switch (n)
            {
                // 8XY0
                case 0x0000:
                    V[X] = V[Y];
                    break;
                // 8XY1
                case 0x0001:
                    V[X] |= V[Y];
                    break;
                // 8XY2
                case 0x0002:
                    V[X] &= V[Y];
                    break;
                // 8XY3
                case 0x0003:
                    V[X] ^= V[Y];
                    break;
                // 8XY4
                case 0x0004:
                    int tmp;
                    tmp = (int)(V[X]) + (int)(V[Y]);
                    if (tmp > 255)
                    {
                        V[0xF] = 1;
                    }
                    else
                    {
                        V[0xF] = 0;
                    }
                    V[X] += V[Y];
                    // V[X] = tmp & 0xFF;
                    break;
                // 8XY5
                case 0x0005:
                    if (V[X] > V[Y])
                    {
                        V[0xF] = 1;
                    }
                    else
                    {
                        V[0xF] = 0;
                    }
                    V[X] -= V[Y];
                    break;
                // 8XY6
                case 0x0006:
                    // V[X] = V[Y];
                    V[0xF] = V[X] & 1;
                    V[X] >>= 1;
                    break;
                // 8XY7
                case 0x0007:
                    if (V[Y] > V[X])
                    {
                        V[0xF] = 1;
                    }
                    else
                    {
                        V[X] = 0;
                    }
                    V[X] = V[Y] - V[X];
                    break;
                // 8XYE
                case 0x000E:
                    // V[X] = V[Y];
                    V[0xF] = V[X] >> 7;
                    V[X] <<= 1;
                    break;
                default:
                    printf("Error executing opcode %x\n", opcode);
                    break;
            }
            break;
        // 9XY0
        case 0x9000:
            if (V[X] != V[Y])
            {
                pc += 2;
            }
            break;
        // ANNN
        case 0xA000:
            I = nnn;
            break;
        case 0xB000:
            pc = nnn + V[0x0];
            break;
        // CXNN
        case 0xC000:
            V[X] = (rand() % 100) & kk;
            break;
        // DXYN
        case 0xD000:
            uint8_t x = V[X] % 64;
            uint8_t y = V[Y] % 32;
            uint8_t pixel;

            V[0xF] = 0;
            for (int i = 0; i < n; i++) {
                pixel = memory[I + i];
                for (int j = 0; j < 8; j++) {
                    if ((pixel & (0x80 >> j)) != 0) {
                        if (display[(x + j) + ((y + i) * 64)] == 1) {
                            V[0xF] = 1;
                        }
                        display[(x + j) + ((y + i) * 64)] ^= 1;
                    }
                }
            }
            drawFlag = 1;
            break;
        case 0xE000:
            switch (kk)
            {
                // EX9E
                case 0x009E:
                    if (keyboard[V[X]] != 0)
                    {
                        pc += 2;
                    }
                    break;
                // EXA1
                case 0x00A1:
                    if (keyboard[V[X]] == 0)
                    {
                        pc += 2;
                    }
                    break;
                default:
                    printf("Error executing opcode %x\n", opcode);
                    break;
            }
            break;
        case 0xF000:
            switch (kk)
            {
                // FX07
                case 0x0007:
                    V[X] = delayTimer;
                    break;
                // FX0A
                case 0x000A:
                    int keyPressed = 0;
                    for (int i = 0; i < 16; i++)
                    {
                        if (keyboard[i])
                        {
                            keyPressed = 1;
                            V[X] = i;
                        }
                    }

                    if (!keyPressed)
                    {
                        pc -= 2;
                    }
                    break;
                // FX15
                case 0x0015:
                    delayTimer = V[X];
                    break;
                // FX18
                case 0x0018:
                    soundTimer = V[X];
                    break;
                // FX1E
                case 0x001E:
                    I += V[X];
                    break;
                // FX29
                case 0x0029:
                    I = V[X] * 5;
                    break;
                // FX33
                case 0x0033:
                    int vx;
                    vx = V[X];
                    memory[I] = (vx - (vx % 100)) / 100;
                    vx -= memory[I] * 100;
                    memory[I + 1] = (vx - (vx % 10)) / 10;
                    vx -= memory[I + 1] * 10;
                    memory[I + 2] = vx;
                    break;
                // FX55
                case 0x0055:
                    for (uint8_t i = 0; i <= X; i++)
                    {
                        memory[I + i] = V[i];
                    }
                    break;
                // FX65
                case 0x0065:
                    for (uint8_t i = 0; i <= X; i++)
                    {
                        V[i] = memory[I + i];
                    }
                    break;
                default:
                    char text[29];
                    sprintf(text, "Error executing opcode %x\n", opcode);
                    print(text);
                    getkey();
                    exit(1);
                    break;
            }
            break;
        default:
            printf("Error executing opcode %x\n", opcode);
            break;
    }
}

void print(char* text) {
	dclear(C_WHITE);
	dtext(1, 1, C_BLACK, text);
	dupdate();
}