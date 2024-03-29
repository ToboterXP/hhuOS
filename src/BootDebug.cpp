#include "BootDebug.h"
#include <stdint.h>

namespace BootDebug {




//Uses the QEMU debug output console (port e9) for debug output before boot is completed
void qemuWriteChar(char c) {
	asm volatile("out %%al, $0xe9" : : "al"(c));
}


void qemuWriteString(const char* str) {
	while (*str) {
		qemuWriteChar(*str);
		str++;
	}
}


void qemuWriteNumber(uint64_t n) {
	uint8_t i = 64;
	bool started = false;
	do {
		i-=4;
		
		uint8_t curr = (n >> i) & 0xf;
		
		if (curr > 0) started = true;
		
		if (started) {
			if (curr >= 10) {
				qemuWriteChar('A'-10+curr);
			} else {
				qemuWriteChar('0'+curr);
			}
		}
	} while (i>0);
	
	if (!started) qemuWriteChar('0');

}

void halt() {
	asm volatile ("cli");
	asm volatile ("hlt");
}

}