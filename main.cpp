#include <Windows.h>
#include <cstdint>
#include <iostream>
#include "rzctl.h"
#include <Psapi.h>

int main() {
	if (!rzctl::init()) {
		printf("Failed to initalize rzctl");
		return 1;
	}

	printf("Press CTRL + C to exit\n");

	while (true) {
	 
		rzctl::mouse_move(10, 10);

		// Sleep is needed to avoid razer service overflowing, which delays all your inputs
		Sleep(1);
	}

	return 0;
}