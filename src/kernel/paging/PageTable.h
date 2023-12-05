#ifndef HHUOS_PAGETABLE_H
#define HHUOS_PAGETABLE_H

#include <stdint.h>

namespace Kernel {

namespace PageTable {
	
enum class PageDirectoryFlag : uint16_t {
	PRESENT = 0x01,
	READ_WRITE = 0x02,
	USER_SUPERVISOR = 0x04,
	WRITE_THROUGH = 0x08,
	CACHE_DISABLE = 0x10,
	ACCESSED = 0x20,
	PAGE_SIZE = 0x80
};

enum class PageTableFlag : uint16_t {
	PRESENT = 0x01,
	READ_WRITE = 0x02,
	USER_SUPERVISOR = 0x04,
	WRITE_THROUGH = 0x08,
	CACHE_DISABLE = 0x10,
	ACCESSED = 0x20,
	DIRTY = 0x40,
	PAGE_ATTRIBUTE_TABLE = 0x80,
	GLOBAL = 0x100
};

typedef uint64_t PageTableEntry;
typedef PageTableEntry PageTable[512] ;

typedef uint64_t PageDirectoryEntry;
typedef PageDirectoryEntry PageDirectory[512] ;

typedef uint64_t PageDirectoryPointerEntry;
typedef PageDirectoryPointerEntry PageDirectoryPointerTable[4];	

uint8_t GetPageDirectoryIndexFromAddress(uint32_t address);
uint16_t GetPageTableIndexFromAddress(uint32_t address);
uint16_t GetPagendexFromAddress(uint32_t address);

void SetPageDirectoryPointerEntry(PageDirectoryPointerTable* pdpt, uint32_t virtualAddress, PageDirectoryPointerEntry value);
void SetPageDirectoryEntry(PageDirectoryPointerTable* pdpt, uint32_t virtualAddress, PageDirectoryEntry value);
void SetPageTableEntry(PageDirectoryPointerTable* pdpt, uint32_t virtualAddress, PageTableEntry value);

PageDirectoryPointerEntry GetPageDirectoryPointerEntry(PageDirectoryPointerTable* pdpt, uint32_t virtualAddress);
PageDirectoryEntry GetPageDirectoryEntry(PageDirectoryPointerTable* pdpt, uint32_t virtualAddress);
PageTableEntry GetPageTableEntry(PageDirectoryPointerTable* pdpt, uint32_t virtualAddress);


//Must be 4KiB aligned
PageDirectoryEntry MakePageDirectoryEntry(uint64_t physicalAddress); 
PageTableEntry MakePageTableEntry(uint64_t physicalAddress);

bool GetPageDirectoryFlag(PageDirectoryEntry entry, PageDirectoryFlag flag);
bool GetPageTableFlag(PageTableEntry entry, PageTableFlag flag);

PageDirectoryEntry SetPageDirectoryFlag(PageDirectoryEntry entry, PageDirectoryFlag flags, bool value);
PageTableEntry SetPageTableFlag(PageTableEntry entry, PageTableFlag flag, bool value);

}


}

#endif