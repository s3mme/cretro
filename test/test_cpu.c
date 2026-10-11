#include <criterion/criterion.h>
#include <string.h>

#include "../src/cpu.h"

#define PC_START 0x200

static void setup(void) {
	memset(&m, 0, sizeof(m));
	m.PC = PC_START;
}

TestSuite(cpu, .init = setup);

// 00E0 - CLS
Test(cpu, OPC_00E0_clears_display) {
	m.video[0]					   = 0xFFFFFFFF;
	m.video[SCREEN_DIMENSIONS - 1] = 0xFFFFFFFF;

	OPC_00E0();

	cr_assert_eq(m.video[0], 0);
	cr_assert_eq(m.video[SCREEN_DIMENSIONS - 1], 0);
}

// 00EE - RET
Test(cpu, OPC_00EE_returns_from_subroutine) {
	m.stack[0] = 0x300;
	m.SP	   = 1;

	OPC_00EE();

	cr_assert_eq(m.PC, 0x300);
	cr_assert_eq(m.SP, 0);
}

// 1nnn - JP addr
Test(cpu, OPC_1nnn_jumps_to_address) {
	m.opcode = 0x1ABC;

	OPC_1nnn();

	cr_assert_eq(m.PC, 0xABC);
}

// 2nnn - CALL addr
Test(cpu, OPC_2nnn_calls_subroutine) {
	m.opcode = 0x2400;

	OPC_2nnn();

	cr_assert_eq(m.stack[0], PC_START);
	cr_assert_eq(m.SP, 1);
	cr_assert_eq(m.PC, 0x400);
}

// 3xkk - SE Vx, byte
Test(cpu, OPC_3xkk_skips_when_equal) {
	m.opcode	   = 0x3155;
	m.registers[1] = 0x55;

	OPC_3xkk();

	cr_assert_eq(m.PC, PC_START + 2);
}

Test(cpu, OPC_3xkk_no_skip_when_not_equal) {
	m.opcode	   = 0x3155;
	m.registers[1] = 0x44;

	OPC_3xkk();

	cr_assert_eq(m.PC, PC_START);
}

// 4xkk - SNE Vx, byte
Test(cpu, OPC_4xkk_skips_when_not_equal) {
	m.opcode	   = 0x4155;
	m.registers[1] = 0x44;

	OPC_4xkk();

	cr_assert_eq(m.PC, PC_START + 2);
}

Test(cpu, OPC_4xkk_no_skip_when_equal) {
	m.opcode	   = 0x4155;
	m.registers[1] = 0x55;

	OPC_4xkk();

	cr_assert_eq(m.PC, PC_START);
}

// 5xy0 - SE Vx, Vy
Test(cpu, OPC_5xy0_skips_when_registers_equal) {
	m.opcode	   = 0x5120;
	m.registers[1] = 0xAA;
	m.registers[2] = 0xAA;

	OPC_5xy0();

	cr_assert_eq(m.PC, PC_START + 2);
}

Test(cpu, OPC_5xy0_no_skip_when_registers_differ) {
	m.opcode	   = 0x5120;
	m.registers[1] = 0xAA;
	m.registers[2] = 0xBB;

	OPC_5xy0();

	cr_assert_eq(m.PC, PC_START);
}

// 6xkk - LD Vx, byte
Test(cpu, OPC_6xkk_loads_byte_into_register) {
	m.opcode = 0x63AB;

	OPC_6xkk();

	cr_assert_eq(m.registers[3], 0xAB);
}

// 7xkk - ADD Vx, byte
Test(cpu, OPC_7xkk_adds_byte_to_register) {
	m.opcode	   = 0x7210;
	m.registers[2] = 0x05;

	OPC_7xkk();

	cr_assert_eq(m.registers[2], 0x15);
}

Test(cpu, OPC_7xkk_wraps_on_overflow) {
	m.opcode	   = 0x7201;
	m.registers[2] = 0xFF;

	OPC_7xkk();

	cr_assert_eq(m.registers[2], 0x00);
}

// 8xy0 - LD Vx, Vy
Test(cpu, OPC_8xy0_stores_vy_in_vx) {
	m.opcode	   = 0x8120;
	m.registers[2] = 0x42;

	OPC_8xy0();

	cr_assert_eq(m.registers[1], 0x42);
}

// 8xy1 - OR Vx, Vy
Test(cpu, OPC_8xy1_bitwise_or) {
	m.opcode	   = 0x8121;
	m.registers[1] = 0x0F;
	m.registers[2] = 0xF0;

	OPC_8xy1();

	cr_assert_eq(m.registers[1], 0xFF);
}

// 8xy2 - AND Vx, Vy
Test(cpu, OPC_8xy2_bitwise_and) {
	m.opcode	   = 0x8122;
	m.registers[1] = 0x0F;
	m.registers[2] = 0xF0;

	OPC_8xy2();

	cr_assert_eq(m.registers[1], 0x00);
}

// 8xy3 - XOR Vx, Vy
Test(cpu, OPC_8xy3_bitwise_xor) {
	m.opcode	   = 0x8123;
	m.registers[1] = 0xFF;
	m.registers[2] = 0x0F;

	OPC_8xy3();

	cr_assert_eq(m.registers[1], 0xF0);
}

// 8xy4 - ADD Vx, Vy (with carry)
Test(cpu, OPC_8xy4_add_no_carry) {
	m.opcode	   = 0x8124;
	m.registers[1] = 0x10;
	m.registers[2] = 0x20;

	OPC_8xy4();

	cr_assert_eq(m.registers[1], 0x30);
	cr_assert_eq(m.registers[0xF], 0);
}

Test(cpu, OPC_8xy4_add_with_carry) {
	m.opcode	   = 0x8124;
	m.registers[1] = 0xFF;
	m.registers[2] = 0x02;

	OPC_8xy4();

	cr_assert_eq(m.registers[1], 0x01);
	cr_assert_eq(m.registers[0xF], 1);
}

// 8xy5 - SUB Vx, Vy
Test(cpu, OPC_8xy5_sub_no_borrow) {
	m.opcode	   = 0x8125;
	m.registers[1] = 0x30;
	m.registers[2] = 0x10;

	OPC_8xy5();

	cr_assert_eq(m.registers[1], 0x20);
	cr_assert_eq(m.registers[0xF], 1);
}

Test(cpu, OPC_8xy5_sub_with_borrow) {
	m.opcode	   = 0x8125;
	m.registers[1] = 0x10;
	m.registers[2] = 0x30;

	OPC_8xy5();

	cr_assert_eq(m.registers[1], (uint8_t) (0x10 - 0x30));
	cr_assert_eq(m.registers[0xF], 0);
}

// 8xy6 - SHR Vx
Test(cpu, OPC_8xy6_shift_right_lsb_set) {
	m.opcode	   = 0x8106;
	m.registers[1] = 0x03;

	OPC_8xy6();

	cr_assert_eq(m.registers[1], 0x01);
	cr_assert_eq(m.registers[0xF], 1);
}

Test(cpu, OPC_8xy6_shift_right_lsb_clear) {
	m.opcode	   = 0x8106;
	m.registers[1] = 0x04;

	OPC_8xy6();

	cr_assert_eq(m.registers[1], 0x02);
	cr_assert_eq(m.registers[0xF], 0);
}

// 8xy7 - SUBN Vx, Vy
Test(cpu, OPC_8xy7_subn_no_borrow) {
	m.opcode	   = 0x8127;
	m.registers[1] = 0x10;
	m.registers[2] = 0x30;

	OPC_8xy7();

	cr_assert_eq(m.registers[1], 0x20);
	cr_assert_eq(m.registers[0xF], 1);
}

Test(cpu, OPC_8xy7_subn_with_borrow) {
	m.opcode	   = 0x8127;
	m.registers[1] = 0x30;
	m.registers[2] = 0x10;

	OPC_8xy7();

	cr_assert_eq(m.registers[1], (uint8_t) (0x10 - 0x30));
	cr_assert_eq(m.registers[0xF], 0);
}

// 8xyE - SHL Vx
Test(cpu, OPC_8xyE_shift_left_msb_set) {
	m.opcode	   = 0x810E;
	m.registers[1] = 0x80;

	OPC_8xyE();

	cr_assert_eq(m.registers[1], 0x00);
	cr_assert_eq(m.registers[0xF], 1);
}

Test(cpu, OPC_8xyE_shift_left_msb_clear) {
	m.opcode	   = 0x810E;
	m.registers[1] = 0x40;

	OPC_8xyE();

	cr_assert_eq(m.registers[1], 0x80);
	cr_assert_eq(m.registers[0xF], 0);
}

// 9xy0 - SNE Vx, Vy
Test(cpu, OPC_9xy0_skips_when_not_equal) {
	m.opcode	   = 0x9120;
	m.registers[1] = 0xAA;
	m.registers[2] = 0xBB;

	OPC_9xy0();

	cr_assert_eq(m.PC, PC_START + 2);
}

Test(cpu, OPC_9xy0_no_skip_when_equal) {
	m.opcode	   = 0x9120;
	m.registers[1] = 0xAA;
	m.registers[2] = 0xAA;

	OPC_9xy0();

	cr_assert_eq(m.PC, PC_START);
}

// Annn - LD I, addr
Test(cpu, OPC_Annn_sets_index) {
	m.opcode = 0xA123;

	OPC_Annn();

	cr_assert_eq(m.index, 0x123);
}

// Bnnn - JP V0, addr
Test(cpu, OPC_Bnnn_jumps_with_offset) {
	m.opcode	   = 0xB300;
	m.registers[0] = 0x10;

	OPC_Bnnn();

	cr_assert_eq(m.PC, 0x310);
}

// Ex9E - SKP Vx
Test(cpu, OPC_Ex9E_skips_when_key_pressed) {
	m.opcode	   = 0xE19E;
	m.registers[1] = 0x05;
	m.keys[0x05]   = 1;

	OPC_Ex9E();

	cr_assert_eq(m.PC, PC_START + 2);
}

Test(cpu, OPC_Ex9E_no_skip_when_key_not_pressed) {
	m.opcode	   = 0xE19E;
	m.registers[1] = 0x05;
	m.keys[0x05]   = 0;

	OPC_Ex9E();

	cr_assert_eq(m.PC, PC_START);
}

// ExA1 - SKNP Vx
Test(cpu, OPC_ExA1_skips_when_key_not_pressed) {
	m.opcode	   = 0xE1A1;
	m.registers[1] = 0x05;
	m.keys[0x05]   = 0;

	OPC_ExA1();

	cr_assert_eq(m.PC, PC_START + 2);
}

Test(cpu, OPC_ExA1_no_skip_when_key_pressed) {
	m.opcode	   = 0xE1A1;
	m.registers[1] = 0x05;
	m.keys[0x05]   = 1;

	OPC_ExA1();

	cr_assert_eq(m.PC, PC_START);
}

// Fx07 - LD Vx, DT
Test(cpu, OPC_Fx07_reads_delay_timer) {
	m.opcode	  = 0xF107;
	m.delay_timer = 42;

	OPC_Fx07();

	cr_assert_eq(m.registers[1], 42);
}

// Fx0A - LD Vx, K
Test(cpu, OPC_Fx0A_waits_for_keypress) {
	m.opcode = 0xF10A;

	OPC_Fx0A();

	cr_assert_eq(m.PC, PC_START - 2, "PC should rewind when no key pressed");
}

Test(cpu, OPC_Fx0A_stores_pressed_key) {
	m.opcode  = 0xF10A;
	m.keys[7] = 1;

	OPC_Fx0A();

	cr_assert_eq(m.registers[1], 7);
	cr_assert_eq(m.PC, PC_START, "PC should not rewind when key is pressed");
}

// Fx15 - LD DT, Vx
Test(cpu, OPC_Fx15_sets_delay_timer) {
	m.opcode	   = 0xF115;
	m.registers[1] = 30;

	OPC_Fx15();

	cr_assert_eq(m.delay_timer, 30);
}

// Fx18 - LD ST, Vx
Test(cpu, OPC_Fx18_sets_sound_timer) {
	m.opcode	   = 0xF118;
	m.registers[1] = 25;

	OPC_Fx18();

	cr_assert_eq(m.sound_timer, 25);
}

// Fx1E - ADD I, Vx
Test(cpu, OPC_Fx1E_adds_vx_to_index) {
	m.opcode	   = 0xF11E;
	m.index		   = 0x100;
	m.registers[1] = 0x10;

	OPC_Fx1E();

	cr_assert_eq(m.index, 0x110);
}

// Fx29 - LD F, Vx
Test(cpu, OPC_Fx29_sets_index_to_font_sprite) {
	m.opcode	   = 0xF129;
	m.registers[1] = 0x0A;

	OPC_Fx29();

	cr_assert_eq(m.index, 5 * 0x0A);
}

// Fx33 - LD B, Vx (BCD)
Test(cpu, OPC_Fx33_stores_bcd) {
	m.opcode	   = 0xF133;
	m.registers[1] = 254;
	m.index		   = 0x300;

	OPC_Fx33();

	cr_assert_eq(m.memory[0x300], 2);
	cr_assert_eq(m.memory[0x301], 5);
	cr_assert_eq(m.memory[0x302], 4);
}

// Fx55 - LD [I], Vx
Test(cpu, OPC_Fx55_stores_registers_to_memory) {
	m.opcode	   = 0xF255;
	m.index		   = 0x300;
	m.registers[0] = 0xAA;
	m.registers[1] = 0xBB;
	m.registers[2] = 0xCC;

	OPC_Fx55();

	cr_assert_eq(m.memory[0x300], 0xAA);
	cr_assert_eq(m.memory[0x301], 0xBB);
	cr_assert_eq(m.memory[0x302], 0xCC);
}

// Fx65 - LD Vx, [I]
Test(cpu, OPC_Fx65_loads_memory_to_registers) {
	m.opcode		= 0xF265;
	m.index			= 0x300;
	m.memory[0x300] = 0x11;
	m.memory[0x301] = 0x22;
	m.memory[0x302] = 0x33;

	OPC_Fx65();

	cr_assert_eq(m.registers[0], 0x11);
	cr_assert_eq(m.registers[1], 0x22);
	cr_assert_eq(m.registers[2], 0x33);
}
