#include "chip-8.h"
#include <iostream>

int main(int argc, char *argv[]) {
	if (argc == 0) { 
		std::cout << "Please select rom!" << std::endl;
		return 0;
	}
	Machine machine(argv[1]);
	return machine.run();
};
