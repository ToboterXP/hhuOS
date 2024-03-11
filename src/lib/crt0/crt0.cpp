#include <cstdint>
#include <cstdarg>
#include "lib/util/base/operators.h"
#include "lib/util/base/Constants.h"
#include "lib/util/base/FreeListMemoryManager.h"

// Export functions
extern "C" {
void initMemoryManager(uint8_t *startAddress, uint8_t *endAddress);
void _exit(int32_t);
}

void initMemoryManager(uint8_t *startAddress, uint8_t *endAddress) {
    auto *memoryManager = new (reinterpret_cast<void*>(Util::USER_SPACE_MEMORY_MANAGER_ADDRESS)) Util::FreeListMemoryManager();
    memoryManager->initialize(startAddress, endAddress);
}

uint16_t systemCall(uint16_t code, uint32_t paramCount...) {
    va_list args;
    va_start(args, paramCount);
    uint16_t result;

    auto eaxValue = static_cast<uint64_t>(code | (paramCount << 16u));
    auto ebxValue = reinterpret_cast<uint64_t>(args);
    auto ecxValue = reinterpret_cast<uint64_t>(&result);

    asm volatile (
    "movq %0, %%rax;"
    "movq %1, %%rbx;"
    "movq %2, %%rcx;"
    "int $0x86;"
    : :
    "g"(eaxValue),
    "g"(ebxValue),
    "g"(ecxValue));

    va_end(args);
    return result;
}

void _exit(int32_t exitCode) {
    systemCall(1, 1, exitCode);
}