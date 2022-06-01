#include <stdint.h>
#include <iostream>

struct CPU {
	uint8_t V[16];              //registers
	uint16_t pc;                //pc register	
	uint16_t I;                 //address register	
	uint8_t sp;                 //stack register	
	uint8_t delayTimer;
	uint8_t soundTimer;
	uint16_t opcode;
};

class Machine {
	CPU cpu;
	uint8_t memory[4096];
	uint8_t key[16];
	uint8_t graphics[32][64];
	uint16_t stack[16];
	bool wait;
	private:
		void keyProcess();
		void clearDisplay();
		void loadRom(std::string path);
		void init();
		void update();
		void draw();
		void debug();
	public:
		Machine(std::string path);
		int run();
};


