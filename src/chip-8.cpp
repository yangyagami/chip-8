#include "chip-8.h"
#include <cstdio>
#include <cwchar>
#include <fstream>
#include <assert.h>
#include <raylib.h>

Machine::Machine(std::string path) {
	init();
	loadRom(path);
	InitWindow(64 * 10, 32 * 10, "chip-8 emulator");
	SetTargetFPS(480);
}

void Machine::loadRom(std::string path) {
	int index = 512;
	std::ifstream in;
	in.open(path);
	assert(in.is_open());
	char c;
	while (in.get(c)) {
		memory[index] = c;
		index++;
		if (index >= 4095) {
			std::cout << "Memory outside!!!" << std::endl;
			std::cout << index - 512 << std::endl;
			assert(false);
		}
	}
	std::cout << index - 512 << std::endl;
	in.close();
}

void Machine::debug() {
	printf("PC     : 0x%x\n", cpu.pc);
	printf("SP     : 0x%x\n", cpu.sp);
	printf("opcode : 0x%x\n", cpu.opcode);
	printf("I      : 0x%x\n", cpu.I);
}

void Machine::init() {
	wait = false;
	cpu.pc = 512;
	cpu.sp = 0;
	cpu.I = 0;
	cpu.opcode = 0;
	cpu.delayTimer = 0;
	cpu.soundTimer = 0;
	for (int i = 0; i < 16; i++) {
		cpu.V[i] = 0;
		stack[i] = 0;
		key[i] = 0;
	}
	for (int i = 0; i < 4096; i++) {
		memory[i] = 0;
	}
	for (int y = 0; y < 32; y++) {
		for (int x = 0; x < 64; x++) {
			graphics[y][x] = 0;
		}
	}

	int index = 0;
	// 0
	memory[index + 0] = 0xF0;
	memory[index + 1] = 0x90;
	memory[index + 2] = 0x90;
	memory[index + 3] = 0x90;
	memory[index + 4] = 0xF0;
	index += 5;

	// 1
	memory[index + 0] = 0x20;
	memory[index + 1] = 0x60;
	memory[index + 2] = 0x20;
	memory[index + 3] = 0x20;
	memory[index + 4] = 0x70;
	index += 5;

	// 2
	memory[index + 0] = 0xF0;
	memory[index + 1] = 0x10;
	memory[index + 2] = 0xF0;
	memory[index + 3] = 0x80;
	memory[index + 4] = 0xF0;
	index += 5;

	// 3
	memory[index + 0] = 0xF0;
	memory[index + 1] = 0x10;
	memory[index + 2] = 0xF0;
	memory[index + 3] = 0x10;
	memory[index + 4] = 0xF0;
	index += 5;

	// 4
	memory[index + 0] = 0x90;
	memory[index + 1] = 0x90;
	memory[index + 2] = 0xF0;
	memory[index + 3] = 0x10;
	memory[index + 4] = 0x10;
	index += 5;

	// 5
	memory[index + 0] = 0xF0;
	memory[index + 1] = 0x80;
	memory[index + 2] = 0xF0;
	memory[index + 3] = 0x10;
	memory[index + 4] = 0xF0;
	index += 5;

	// 6
	memory[index + 0] = 0xF0;
	memory[index + 1] = 0x80;
	memory[index + 2] = 0xF0;
	memory[index + 3] = 0x90;
	memory[index + 4] = 0xF0;
	index += 5;

	// 7
	memory[index + 0] = 0xF0;
	memory[index + 1] = 0x10;
	memory[index + 2] = 0x20;
	memory[index + 3] = 0x40;
	memory[index + 4] = 0x40;
	index += 5;

	// 8
	memory[index + 0] = 0xF0;
	memory[index + 1] = 0x90;
	memory[index + 2] = 0xF0;
	memory[index + 3] = 0x90;
	memory[index + 4] = 0xF0;
	index += 5;

	// 9
	memory[index + 0] = 0xF0;
	memory[index + 1] = 0x90;
	memory[index + 2] = 0xF0;
	memory[index + 3] = 0x10;
	memory[index + 4] = 0xF0;
	index += 5;

	// A
	memory[index + 0] = 0xF0;
	memory[index + 1] = 0x90;
	memory[index + 2] = 0xF0;
	memory[index + 3] = 0x90;
	memory[index + 4] = 0x90;
	index += 5;

	// B
	memory[index + 0] = 0xE0;
	memory[index + 1] = 0x90;
	memory[index + 2] = 0xE0;
	memory[index + 3] = 0x90;
	memory[index + 4] = 0xE0;
	index += 5;

	// C
	memory[index + 0] = 0xF0;
	memory[index + 1] = 0x80;
	memory[index + 2] = 0x80;
	memory[index + 3] = 0x80;
	memory[index + 4] = 0xF0;
	index += 5;

	// D
	memory[index + 0] = 0xE0;
	memory[index + 1] = 0x90;
	memory[index + 2] = 0x90;
	memory[index + 3] = 0x90;
	memory[index + 4] = 0xE0;
	index += 5;

	// E
	memory[index + 0] = 0xF0;
	memory[index + 1] = 0x80;
	memory[index + 2] = 0xF0;
	memory[index + 3] = 0x80;
	memory[index + 4] = 0xF0;
	index += 5;

	// F
	memory[index + 0] = 0xF0;
	memory[index + 1] = 0x80;
	memory[index + 2] = 0xF0;
	memory[index + 3] = 0x80;
	memory[index + 4] = 0x80;
	index += 5;
}

void Machine::clearDisplay() {
	for (int y = 0; y < 32; y++) {
		for (int x = 0;  x < 64; x++) {
			graphics[y][x] = 0;
		}
	}
}

void Machine::keyProcess() {
	for (int i = 0; i < 16; i++) {
		key[i] = 0;
	}

	if (IsKeyDown(KEY_KP_7)) {
		key[1] = 1;
	}
	if (IsKeyDown(KEY_KP_8)) {
		key[2] = 1;
	}
	if (IsKeyDown(KEY_KP_9)) {
		key[3] = 1;
	}
	if (IsKeyDown(KEY_KP_4)) {
		key[4] = 1;
	}
	if (IsKeyDown(KEY_KP_5)) {
		key[5] = 1;
	}
	if (IsKeyDown(KEY_KP_6)) {
		key[6] = 1;
	}
	if (IsKeyDown(KEY_KP_1)) {
		key[7] = 1;
	}
	if (IsKeyDown(KEY_KP_2)) {
		key[8] = 1;
	}
	if (IsKeyDown(KEY_KP_3)) {
		key[9] = 1;
	}
	if (IsKeyDown(KEY_KP_0)) {
		key[0] = 1;
	}
	if (IsKeyDown(KEY_A)) {
		key[10] = 1;
	}
	if (IsKeyDown(KEY_B)) {
		key[11] = 1;
	}
	if (IsKeyDown(KEY_C)) {
		key[12] = 1;
	}
	if (IsKeyDown(KEY_D)) {
		key[13] = 1;
	}
	if (IsKeyDown(KEY_E)) {
		key[14] = 1;
	}
	if (IsKeyDown(KEY_F)) {
		key[15] = 1;
	}
}

void Machine::update() {
	cpu.opcode = (memory[cpu.pc] << 8) | (memory[cpu.pc + 1]);
	switch (cpu.opcode & 0xF000) {
		case 0x0000:	
			switch (cpu.opcode & 0x0FFF) {
				case 0x00E0:
					clearDisplay();
					cpu.pc += 2;
					break;	
				case 0x00EE:
					cpu.pc = stack[cpu.sp--];
					cpu.pc += 2;
					break;	
				default:
					cpu.pc = (cpu.opcode & 0x0FFF);
					break;
			}
			break;
		case 0x1000:
			cpu.pc = (cpu.opcode & 0x0FFF);
			break;
		case 0x2000:
			stack[++cpu.sp] = cpu.pc;
			cpu.pc = (cpu.opcode & 0x0FFF);
			break;
		case 0x3000:
			{
				if (cpu.V[(cpu.opcode & 0x0F00) >> 8] == (cpu.opcode & 0x00FF)) {
					cpu.pc += 4;
				} else {
					cpu.pc += 2;
				}
			}
			break;
		case 0x4000:
			{
				if (cpu.V[(cpu.opcode & 0x0F00) >> 8] != (cpu.opcode & 0x00FF)) {
					cpu.pc += 4;
				} else {
					cpu.pc += 2;
				}
			}
			break;
		case 0x5000:
			{
				if (((cpu.opcode & 0x000F) == 0) && cpu.V[(cpu.opcode & 0x0F00) >> 8] == cpu.V[(cpu.opcode & 0x00F0) >> 4]) {
					cpu.pc += 4;
				} else {
					cpu.pc += 2;
				}
			}
			break;
		case 0x6000:
			cpu.V[(cpu.opcode & 0x0F00) >> 8] = (cpu.opcode & 0x00FF); 
			cpu.pc += 2;
			break;
		case 0x7000:
			cpu.V[(cpu.opcode & 0x0F00) >> 8] += (cpu.opcode & 0x00FF); 
			cpu.pc += 2;
			break;
		case 0x8000:
			switch (cpu.opcode & 0x000F) {
				case 0x0000:
					cpu.V[(cpu.opcode & 0x0F00) >> 8] = cpu.V[(cpu.opcode & 0x00F0) >> 4];
					cpu.pc += 2;
					break;
				case 0x0001:
					cpu.V[(cpu.opcode & 0x0F00) >> 8] |= cpu.V[(cpu.opcode & 0x00F0) >> 4];
					cpu.pc += 2;
					break;
				case 0x0002:
					cpu.V[(cpu.opcode & 0x0F00) >> 8] &= cpu.V[(cpu.opcode & 0x00F0) >> 4];
					cpu.pc += 2;
					break;
				case 0x0003:
					cpu.V[(cpu.opcode & 0x0F00) >> 8] ^= cpu.V[(cpu.opcode & 0x00F0) >> 4];
					cpu.pc += 2;
					break;
				case 0x0004:
					if (cpu.V[(cpu.opcode & 0x0F00) >> 8] > (255 - cpu.V[(cpu.opcode & 0x00F0) >> 4])) {
						cpu.V[15] = 1;
					} else {
						cpu.V[15] = 0;
					}
					cpu.V[(cpu.opcode & 0x0F00) >> 8] += cpu.V[(cpu.opcode & 0x00F0) >> 4];
					cpu.pc += 2;
					break;
				case 0x0005:
					if (cpu.V[(cpu.opcode & 0x0F00) >> 8] > cpu.V[(cpu.opcode & 0x00F0) >> 4]) {
						cpu.V[15] = 1;
					} else {
						cpu.V[15] = 0;
					}
					cpu.V[(cpu.opcode & 0x0F00) >> 8] -= cpu.V[(cpu.opcode & 0x00F0) >> 4];
					cpu.pc += 2;
					break;
				case 0x0006:
					cpu.V[15] = cpu.V[(cpu.opcode & 0x0F00) >> 8] & 0x01; 
					cpu.V[(cpu.opcode & 0x0F00) >> 8] >>= 1;
					cpu.pc += 2;
					break;
				case 0x0007:
					if (cpu.V[(cpu.opcode & 0x0F00) >> 8] < cpu.V[(cpu.opcode & 0x00F0) >> 4]) {
						cpu.V[15] = 1;
					} else {
						cpu.V[15] = 0;
					}
					cpu.V[(cpu.opcode & 0x0F00) >> 8] = cpu.V[(cpu.opcode & 0x00F0) >> 4] - cpu.V[(cpu.opcode & 0x0F00) >> 8];
					cpu.pc += 2;
					break;
				case 0x000E:
					cpu.V[15] = (cpu.V[(cpu.opcode & 0x0F00) >> 8] >> 7); 
					cpu.V[(cpu.opcode & 0x0F00) >> 8] <<= 1;
					cpu.pc += 2;
					break;

			}
			break;
		case 0x9000:
			if ((cpu.opcode & 0x000F) == 0x0000) {
				if (cpu.V[(cpu.opcode & 0x0F00) >> 8] != cpu.V[(cpu.opcode & 0x00F0) >> 4]) {
					cpu.pc += 4;
				} else {
					cpu.pc += 2;
				}
			}
			break;
		case 0xA000:
			cpu.I = (cpu.opcode & 0x0FFF);
			cpu.pc += 2;
			break;
		case 0xB000:
			cpu.pc = (cpu.opcode & 0x0FFF) + cpu.V[0];
			break;
		case 0xC000:
			cpu.V[(cpu.opcode & 0x0F00) >> 8] = (rand() % 256) & (cpu.opcode & 0x00FF);
			cpu.pc += 2;
			break;
		case 0xD000:
			{
				int index = cpu.I;
				int shift = 0;
				int startY = cpu.V[(cpu.opcode & 0x00F0) >> 4];
				int startX = cpu.V[(cpu.opcode & 0x0F00) >> 8];
				cpu.V[15] = 0;	
				for (int y = startY; y < startY + (cpu.opcode & 0x000F); y++) {
					uint8_t pixel = memory[index++];
					shift = 0;
					for (int x = startX; x < startX + 8; x++) {
						int mx = x;
						int my = y;
						if (mx >= 64) mx %= 64; 
						if (my >= 32) my %= 32; 
						if (mx < 0) mx = (-mx % 64) + 64; 
						if (my < 0) my = (-my % 32) + 32; 
						if ((pixel & (0x80 >> shift))) {
							if (graphics[my][mx] == 1) {
								cpu.V[15] = 1;	
							}
							graphics[my][mx] ^= 1;
						}	
						shift++;
					}
				}
				cpu.pc += 2;
			}
			break;
		case 0xE000:
			switch (cpu.opcode & 0x00FF) {
				case 0x009E:
					if (key[cpu.V[(cpu.opcode & 0x0F00) >> 8]] == 1) {
						cpu.pc += 4;
					} else {
						cpu.pc += 2;
					}
					break;
				case 0x00A1:
					if (key[cpu.V[(cpu.opcode & 0x0F00) >> 8]] != 1) {
						cpu.pc += 4;
					} else {
						cpu.pc += 2;
					}
					break;
			}
			break;
		case 0xF000:
			switch (cpu.opcode & 0x00FF) {
				case 0x0007:
					cpu.V[(cpu.opcode & 0x0F00) >> 8] = cpu.delayTimer;
					cpu.pc += 2;
					break;
				case 0x000A:
					wait = true;
					for (int i = 0; i < 16; i++) {
						if (key[i] == 1) {
							wait = false;	
							cpu.V[(cpu.opcode & 0x0F00) >> 8] = i;
							break;
						}
					}
					if (wait == false)
						cpu.pc += 2;
					break;
				case 0x0015:
					cpu.delayTimer = cpu.V[(cpu.opcode & 0x0F00) >> 8];
					cpu.pc += 2;
					break;
				case 0x0018:
					cpu.soundTimer = cpu.V[(cpu.opcode & 0x0F00) >> 8];
					cpu.pc += 2;
					break;
				case 0x001E:
					cpu.V[15] = (cpu.V[(cpu.opcode & 0x0F00) >> 8] + cpu.I) > 255 ? 1 : 0;
					cpu.I += cpu.V[(cpu.opcode & 0x0F00) >> 8];
					cpu.pc += 2;
					break;
				case 0x0029:
					cpu.I = cpu.V[(cpu.opcode & 0x0F00) >> 8] * 5;
					cpu.pc += 2;
					break;
				case 0x0033:
					memory[cpu.I] = cpu.V[(cpu.opcode & 0x0F00) >> 8] / 100;
					memory[cpu.I + 1] = cpu.V[(cpu.opcode & 0x0F00) >> 8] / 10 % 10;
					memory[cpu.I + 2] = cpu.V[(cpu.opcode & 0x0F00) >> 8] % 10;
					cpu.pc += 2;
					break;
				case 0x0055:
					{
						uint16_t index = cpu.I;
						for (uint8_t i = 0; i <= ((cpu.opcode & 0x0F00) >> 8); i++) {
							memory[index++] = cpu.V[i];
						}
						cpu.I += 1 + ((cpu.opcode & 0x0F00) >> 8);
						cpu.pc += 2;
					}
					break;
				case 0x0065:
					{
						uint16_t index = cpu.I;
						for (uint8_t i = 0; i <= ((cpu.opcode & 0x0F00) >> 8); i++) {
							cpu.V[i] = memory[index++];
						}
						cpu.I += 1 + ((cpu.opcode & 0x0F00) >> 8);
						cpu.pc += 2;
					}
					break;

			}
			break;
		default:
			std::cout << "unknown opcode!!" << std::endl;
			break;
	}
	if (cpu.delayTimer > 0) {
		cpu.delayTimer--;
	}
	if (cpu.soundTimer > 0) {
		cpu.soundTimer--;
	}

}

void Machine::draw() {
	BeginDrawing();
	    ClearBackground(BLACK);
		for (int y = 0; y < 32; y++) {
			for (int x = 0; x < 64; x++) {
				if (graphics[y][x] == 1) {
					DrawRectangle(x * 10, y * 10, 10, 10, WHITE);
				}
			}
		}
	EndDrawing();	
}

int Machine::run() {
	while (!WindowShouldClose()) {
		keyProcess();
		update();
		draw();
		//getchar();
	}
	CloseWindow();
	return 0;
}
