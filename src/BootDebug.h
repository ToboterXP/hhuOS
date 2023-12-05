#ifndef HHUOS_BOOTDEBUG_H
#define HHUOS_BOOTDEBUG_H



//Functions that are able to provide debug output before memory management is set up
namespace BootDebug {
	
	void qemuWriteChar(char c);
	void qemuWriteString(const char* str);

}

#endif