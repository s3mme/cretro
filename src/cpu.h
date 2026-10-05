#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define MEMORY_SIZE		  4096
#define REGISTER_COUNT	  16
#define STACK_LEVELS	  16
#define KEY_SIZE		  16
#define VIDEO_HEIGHT	  32
#define VIDEO_WIDTH		  64
#define SCREEN_DIMENSIONS (VIDEO_WIDTH * VIDEO_HEIGHT)

void rom_load(char const *filename);
void machine_init(void);
void cpu_step(void);

typedef struct machine_t {
	uint8_t memory[MEMORY_SIZE];
	uint8_t registers[REGISTER_COUNT];
	uint16_t stack[STACK_LEVELS];
	uint32_t video[SCREEN_DIMENSIONS];
	uint8_t keys[KEY_SIZE];

	uint16_t index;
	uint16_t SP;
	uint16_t PC;
	uint16_t cycle_count;
	uint16_t opcode;
	uint8_t delay_timer;
	uint8_t sound_timer;
	uint8_t should_beep;
} machine_t;

extern machine_t m;

// Clear the display
void OPC_00E0(void);

// RET: Return from a subroutine.
void OPC_00EE(void);

// JMP nnn: Jump to nnn
void OPC_1nnn(void);

// CALL nnn: call subroutine at nnn
void OPC_2nnn(void);

// SE Vx, byte: Skip next instruction if Vx = kk.
void OPC_3xkk(void);

// SNE Vx, byte: Skip next instruction if register Vx != kk.
void OPC_4xkk(void);

//  SE Vx, Vy: Skip next instruction if register Vx == Vy.
void OPC_5xy0(void);

// LD Vx, byte: Puts the value kk into register Vx
void OPC_6xkk(void);

// ADD Vx, byte: Adds the value kk to the value of register Vx, then stores the result in Vx
void OPC_7xkk(void);

// LD Vx, Vy: Stores the value of register Vy in register Vx.
void OPC_8xy0(void);

// OR Vx, Vy: Set Vx := Vx OR Vy
void OPC_8xy1(void);

// AND Vx, Vy: Set Vx := Vx AND Vy
void OPC_8xy2(void);

// XOR Vx, Vy: Set Vx := Vx XOR Vy
void OPC_8xy3(void);

// ADD Vx, Vy: Set Vx = Vx + Vy, set VF = carry
void OPC_8xy4(void);

// SUB Vx, Vy: Set Vx := Vx - Vy, set VF := NOT borrow.
void OPC_8xy5(void);

// SHR Vx: Set Vx = Vx SHR 1.
void OPC_8xy6(void);

// SUBN Vx, Vy: Set Vx := Vy - Vx, set VF = NOT borrow.
void OPC_8xy7(void);

// SHL Vx: Set Vx = Vx SHL 1
void OPC_8xyE(void);

// SNE Vx, Vy Skip next instruction if Vx != Vy
void OPC_9xy0(void);

//  LD I, addr: Set I = nnn
void OPC_Annn(void);

// JP V0, addr: Jump to location nnn + V0
void OPC_Bnnn(void);

// RND Vx, byte: Set Vx = random byte AND kk
void OPC_Cxkk(void);

// DRW Vx, Vy, nibble: Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
void OPC_Dxyn(void);

// Ex9E - SKP Vx: Skip next instruction if key with the value of Vx is pressed.
void OPC_Ex9E(void);

// ExA1 - SKNP Vx: Skip next instruction if key with the value of Vx is not pressed.
void OPC_ExA1(void);

// Fx07 - LD Vx, DT: Set Vx = delay timer value. The value of DT is placed into Vx.
void OPC_Fx07(void);

// Fx0A - LD Vx, K: Wait for a key press, store the value of the key in Vx.
// All execution stops until a key is pressed, then the value of that key is stored in Vx.
void OPC_Fx0A(void);

// Fx15 - LD DT, Vx: Set delay timer = Vx. Delay Timer is set equal to the value of Vx.
void OPC_Fx15(void);

// Fx18 - LD ST, Vx: Set sound timer = Vx. Sound Timer is set equal to the value of Vx.
void OPC_Fx18(void);

// Fx1E - ADD I, Vx: Set I = I + Vx. The values of I and Vx are added, and the results are stored in I.
void OPC_Fx1E(void);

// Fx29 - LD F, Vx: Set I = location of sprite for digit Vx. The value of I is set to the location for the
// hexadecimal sprite corresponding to the value of Vx.
void OPC_Fx29(void);

// Fx33 - LD B, Vx: Store BCD representation of Vx in memory locations I, I+1, and I+2.
void OPC_Fx33(void);

// Fx55 - LD [I], Vx: Stores V0 to VX in memory starting at address I. I is then set to I + x + 1.
void OPC_Fx55(void);

// Fx65 - LD Vx, [I]: Fills V0 to VX with values from memory starting at address I. I is then set to I + x + 1
void OPC_Fx65(void);

#endif
