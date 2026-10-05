#include <stdio.h>
#include <errno.h>
#include "cpu.h"
#include "sound.h"
#include "logging.h"

#define FONTSET_SIZE		  80
#define FONTSET_START_ADDRESS 0x00
#define START_ADDRESS		  0x200

#define GET_X(opcode)	((uint8_t) ((opcode >> 8) & 0x000F))
#define GET_Y(opcode)	((uint8_t) ((opcode >> 4) & 0x000F))
#define GET_N(opcode)	((uint8_t) (opcode & 0x000F))
#define GET_KK(opcode)	((uint8_t) (opcode & 0x00FF))
#define GET_NNN(opcode) ((uint16_t) (opcode & 0x0FFF))

typedef void (*op_function)(void);

// Function is used for instruction array initialization, not recognized by compiler
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
static void illegal_opcode(void) {
	LOG_ERROR("Unallowed OP Code!");
}
#pragma GCC diagnostic pop

op_function instr_lookup[0xF + 1]		   = {[0 ... 0xF] = illegal_opcode};
op_function zero_prefixed_lookup[0xE + 1]  = {[0 ... 0xE] = illegal_opcode};
op_function eight_prefixed_lookup[0xE + 1] = {[0 ... 0xE] = illegal_opcode};
op_function e_prefixed_lookup[0xE + 1]	   = {[0 ... 0xE] = illegal_opcode};
op_function f_prefixed_lookup[0x65 + 1]	   = {[0 ... 0x65] = illegal_opcode};

uint8_t fontset[FONTSET_SIZE] = {
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
	0xF0, 0x80, 0xF0, 0x80, 0x80, // F
};

machine_t m = {
	{[0 ...(4096 - 1)] = 0},
	{[0 ...(REGISTER_COUNT - 1)] = 0},
	{[0 ...(STACK_LEVELS - 1)] = 0},
	{[0 ...(SCREEN_DIMENSIONS - 1)] = 0},
	{[0 ...(KEY_SIZE - 1)] = 0},
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
};

void rom_load(char const *filename) {
	FILE *f;
	long rom_size;

	// Open ROM
	LOG_INFO("Loading ROM %s", filename);
	f = fopen(filename, "r");
	if (f == NULL) {
		LOG_FATAL("%s", strerror(errno));
		return;
	}

	// Get the number of bytes
	fseek(f, 0L, SEEK_END);
	rom_size = ftell(f);

	LOG_DEBUG("Size of ROM: %ld", rom_size);

	if (rom_size < 0) {
		LOG_FATAL("%s", strerror(errno));
		fclose(f);
		return;
	} else if (((unsigned long) rom_size) >= UINT16_MAX) {
		LOG_FATAL("Size of rom %s (%ld) is larger than maximal allowed size %d", filename, rom_size,
				  UINT16_MAX);
		fclose(f);
		return;
	}

	// reset pointer to beginning of file
	fseek(f, 0L, SEEK_SET);

	size_t bytes_read = fread(&m.memory[START_ADDRESS], sizeof(uint8_t), (size_t) rom_size, f);
	if (bytes_read != (size_t) rom_size) {
		LOG_FATAL("An error occurred while reading bytes from rom");
		fclose(f);
		return;
	}

	LOG_DEBUG("Successfully read %ld B of ROM", rom_size);
	fclose(f);
}

static void zero_prefixed(void) {
	zero_prefixed_lookup[GET_N(m.opcode)]();
}

static void eight_prefixed(void) {
	eight_prefixed_lookup[GET_N(m.opcode)]();
}

static void e_prefixed(void) {
	e_prefixed_lookup[GET_N(m.opcode)]();
}

static void f_prefixed(void) {
	f_prefixed_lookup[GET_KK(m.opcode)]();
}

void machine_init(void) {
	sound_init();

	m.PC = START_ADDRESS;
	srand((unsigned int) time(NULL));

	memcpy(&m.memory[FONTSET_START_ADDRESS], fontset, FONTSET_SIZE);

	// Set up lookup table
	instr_lookup[0x0] = zero_prefixed;
	instr_lookup[0x1] = OPC_1nnn;
	instr_lookup[0x2] = OPC_2nnn;
	instr_lookup[0x3] = OPC_3xkk;
	instr_lookup[0x4] = OPC_4xkk;
	instr_lookup[0x5] = OPC_5xy0;
	instr_lookup[0x6] = OPC_6xkk;
	instr_lookup[0x7] = OPC_7xkk;
	instr_lookup[0x8] = eight_prefixed;
	instr_lookup[0x9] = OPC_9xy0;
	instr_lookup[0xA] = OPC_Annn;
	instr_lookup[0xB] = OPC_Bnnn;
	instr_lookup[0xC] = OPC_Cxkk;
	instr_lookup[0xD] = OPC_Dxyn;
	instr_lookup[0xE] = e_prefixed;
	instr_lookup[0xF] = f_prefixed;

	zero_prefixed_lookup[0x0] = OPC_00E0;
	zero_prefixed_lookup[0xE] = OPC_00EE;

	eight_prefixed_lookup[0x0] = OPC_8xy0;
	eight_prefixed_lookup[0x1] = OPC_8xy1;
	eight_prefixed_lookup[0x2] = OPC_8xy2;
	eight_prefixed_lookup[0x3] = OPC_8xy3;
	eight_prefixed_lookup[0x4] = OPC_8xy4;
	eight_prefixed_lookup[0x5] = OPC_8xy5;
	eight_prefixed_lookup[0x6] = OPC_8xy6;
	eight_prefixed_lookup[0x7] = OPC_8xy7;
	eight_prefixed_lookup[0xE] = OPC_8xyE;

	e_prefixed_lookup[0x1] = OPC_ExA1;
	e_prefixed_lookup[0xE] = OPC_Ex9E;

	f_prefixed_lookup[0x07] = OPC_Fx07;
	f_prefixed_lookup[0x0A] = OPC_Fx0A;
	f_prefixed_lookup[0x15] = OPC_Fx15;
	f_prefixed_lookup[0x18] = OPC_Fx18;
	f_prefixed_lookup[0x1E] = OPC_Fx1E;
	f_prefixed_lookup[0x29] = OPC_Fx29;
	f_prefixed_lookup[0x33] = OPC_Fx33;
	f_prefixed_lookup[0x55] = OPC_Fx55;
	f_prefixed_lookup[0x65] = OPC_Fx65;
}

void cpu_step(void) {
	// Get opcode and increment PC
	m.opcode = (m.memory[m.PC] << 8) | m.memory[m.PC + 1];

	m.PC += 2;

	// Get and Execute Instruction
	(*(instr_lookup[(m.opcode & 0xF000) >> 12]))();
}

// OP-Codes:
void OPC_00E0(void) {
	memset(m.video, 0, sizeof(m.video));
}

void OPC_00EE(void) {
	--m.SP;
	m.PC = m.stack[m.SP];
}

void OPC_1nnn(void) {
	uint16_t address = GET_NNN(m.opcode);

	m.PC = address;
}

void OPC_2nnn(void) {
	m.stack[m.SP] = m.PC;
	++m.SP;

	m.PC = GET_NNN(m.opcode);
}

void OPC_3xkk(void) {
	uint8_t vx	 = GET_X(m.opcode);
	uint8_t byte = GET_KK(m.opcode);

	if (m.registers[vx] == byte) {
		m.PC += 2;
	}
}

void OPC_4xkk(void) {
	uint8_t vx	 = GET_X(m.opcode);
	uint8_t byte = GET_KK(m.opcode);

	if (m.registers[vx] != byte) {
		m.PC += 2;
	}
}

void OPC_5xy0(void) {
	uint8_t vx = GET_X(m.opcode);
	uint8_t vy = GET_Y(m.opcode);

	if (m.registers[vx] == m.registers[vy]) {
		m.PC += 2;
	}
}

void OPC_6xkk(void) {
	uint8_t vx	 = GET_X(m.opcode);
	uint8_t byte = GET_KK(m.opcode);

	m.registers[vx] = byte;
}

void OPC_7xkk(void) {
	uint8_t vx	 = GET_X(m.opcode);
	uint8_t byte = GET_KK(m.opcode);

	m.registers[vx] += byte;
}

void OPC_8xy0(void) {
	uint8_t vx = GET_X(m.opcode);
	uint8_t vy = GET_Y(m.opcode);

	m.registers[vx] = m.registers[vy];
}

void OPC_8xy1(void) {
	uint8_t vx = GET_X(m.opcode);
	uint8_t vy = GET_Y(m.opcode);

	m.registers[vx] |= m.registers[vy];
}

void OPC_8xy2(void) {
	uint8_t vx = GET_X(m.opcode);
	uint8_t vy = GET_Y(m.opcode);

	m.registers[vx] &= m.registers[vy];
}

void OPC_8xy3(void) {
	uint8_t vx = GET_X(m.opcode);
	uint8_t vy = GET_Y(m.opcode);

	m.registers[vx] ^= m.registers[vy];
}

void OPC_8xy4(void) {
	uint8_t vx = GET_X(m.opcode);
	uint8_t vy = GET_Y(m.opcode);

	uint16_t sum = m.registers[vx] + m.registers[vy];

	m.registers[0xF] = sum > 255;

	m.registers[vx] = (uint8_t) (sum & 0xFF);
}

void OPC_8xy5(void) {
	uint8_t vx = GET_X(m.opcode);
	uint8_t vy = GET_Y(m.opcode);

	m.registers[0xF] = m.registers[vx] > m.registers[vy];

	m.registers[vx] -= m.registers[vy];
}

void OPC_8xy6(void) {
	uint8_t vx = GET_X(m.opcode);

	// Save LSB in VF
	m.registers[0xF] = m.registers[vx] & 0x1;

	m.registers[vx] = m.registers[vx] >> 1;
}

void OPC_8xy7(void) {
	uint8_t vx = GET_X(m.opcode);
	uint8_t vy = GET_Y(m.opcode);

	m.registers[0xF] = m.registers[vy] > m.registers[vx];

	m.registers[vx] = m.registers[vy] - m.registers[vx];
}

void OPC_8xyE(void) {
	uint8_t vx = GET_X(m.opcode);

	// Save MSB in VF because if msb = 1 set VF to 1
	// analogously with msb = 0
	m.registers[0xF] = (m.registers[vx] & 0b10000000) >> 7;

	m.registers[vx] = m.registers[vx] << 1;
}

void OPC_9xy0(void) {
	uint8_t vx = GET_X(m.opcode);
	uint8_t vy = GET_Y(m.opcode);

	if (m.registers[vx] != m.registers[vy]) {
		m.PC += 2;
	}
}

void OPC_Annn(void) {
	uint16_t reg_addr = GET_NNN(m.opcode);

	m.index = reg_addr;
}

void OPC_Bnnn(void) {
	uint16_t reg_addr = GET_NNN(m.opcode);
	uint8_t offset	  = m.registers[0];

	m.PC = reg_addr + offset;
}

void OPC_Cxkk(void) {
	uint8_t vx	   = GET_X(m.opcode);
	uint8_t byte   = GET_KK(m.opcode);
	uint8_t random = (uint8_t) (rand() & byte);

	m.registers[vx] = random & byte;
}

void OPC_Dxyn(void) {
	uint_fast8_t vx		= (m.opcode & 0x0F00u) >> 8u;
	uint_fast8_t vy		= (m.opcode & 0x00F0u) >> 4u;
	uint_fast8_t height = m.opcode & 0x000Fu;

	// Wrap if going beyond screen boundaries
	uint_fast8_t vx_value = m.registers[vx] % VIDEO_WIDTH;
	uint_fast8_t vy_value = m.registers[vy] % VIDEO_HEIGHT;

	m.registers[0xF] = 0;

	for (uint_fast8_t row = 0; row < height; ++row) {
		uint_fast8_t sprite_pixel = m.memory[m.index + row];

		for (uint_fast8_t col = 0; col < 8; ++col) {
			uint_fast8_t bit = sprite_pixel & (0b10000000 >> col);
			uint32_t *pixel	 = &m.video[(vy_value + row) * VIDEO_WIDTH + (vx_value + col)];

			// Sprite pixel is set
			if (bit) {
				if (*pixel == 0xFFFFFFFF) { // pixel on screen => set carry/collision bit
					m.registers[0xF] = 1;
				}

				*pixel ^= 0xFFFFFFFF;
			}
		}
	}
}

void OPC_Ex9E(void) {
	uint8_t vx	= GET_X(m.opcode);
	uint8_t key = m.registers[vx];

	if (m.keys[key]) {
		m.PC += 2;
	}
}

void OPC_ExA1(void) {
	uint8_t vx	= GET_X(m.opcode);
	uint8_t key = m.registers[vx];

	if (!m.keys[key]) {
		m.PC += 2;
	}
}

void OPC_Fx07(void) {
	uint8_t vx		= GET_X(m.opcode);
	m.registers[vx] = m.delay_timer;
}

void OPC_Fx0A(void) {
	uint8_t vx = GET_X(m.opcode);

	for (int i = 0; i < 16; ++i) {
		if (m.keys[i]) {
			m.registers[vx] = (uint8_t) i;
			return;
		}
	}

	m.PC -= 2;
}

void OPC_Fx15(void) {
	uint8_t vx = GET_X(m.opcode);

	m.delay_timer = m.registers[vx];
}

void OPC_Fx18(void) {
	uint8_t vx = GET_X(m.opcode);

	m.sound_timer = m.registers[vx];
}

void OPC_Fx1E(void) {
	uint8_t vx = GET_X(m.opcode);

	m.index += m.registers[vx];
}

void OPC_Fx29(void) {
	uint8_t vx	  = GET_X(m.opcode);
	uint8_t value = m.registers[vx];

	m.index = FONTSET_START_ADDRESS + (5 * value); // 5 = font char width
}

void OPC_Fx33(void) {
	uint8_t vx	  = GET_X(m.opcode);
	uint8_t value = m.registers[vx];

	// 1s
	m.memory[m.index + 2] = value % 10;
	value /= 10;

	// 10s
	m.memory[m.index + 1] = value % 10;
	value /= 10;

	// 100s
	m.memory[m.index] = value % 10;
}

void OPC_Fx55(void) {
	uint8_t vx = GET_X(m.opcode);

	memcpy(&m.memory[m.index], m.registers, vx + 1);
}

void OPC_Fx65(void) {
	uint8_t vx = GET_X(m.opcode);

	memcpy(m.registers, &m.memory[m.index], vx + 1);
}
