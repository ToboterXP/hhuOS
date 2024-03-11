#include "BootDebug.h"
#include <stdint.h>

namespace BootDebug {




//Uses the QEMU debug output console (port e9) for debug output before boot is completed
void qemuWriteChar(char c) {
	//asm volatile("out %0, $0xe9" : : "al"(c));
}


void qemuWriteString(const char* str) {
	while (*str) {
		qemuWriteChar(*str);
		str++;
	}
}

}