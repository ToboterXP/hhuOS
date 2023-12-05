#include "PageTable.h"


namespace Kernel {

namespace PageTable {
	
uint8_t GetPageDirectoryIndexFromAddress(uint32_t address) {
	return (address>>30)&0b11;
}

uint16_t GetPageTableIndexFromAddress(uint32_t address) {
	return (address>>21)&0x1ff;
}

uint16_t GetPageIndexFromAddress(uint32_t address) {
	return (address>>12)&0x1ff;
}



void SetPageDirectoryPointerEntry(PageDirectoryPointerTable* pdpt, uint32_t virtualAddress, PageDirectoryPointerEntry value) {
	(*pdpt)[GetPageDirectoryIndexFromAddress(virtualAddress)] = value | 1; //automatically marked as present
}

PageDirectoryPointerEntry GetPageDirectoryPointerEntry(PageDirectoryPointerTable* pdpt, uint32_t virtualAddress) {
	return (*pdpt)[GetPageDirectoryIndexFromAddress(virtualAddress)] & (~1);
}


void SetPageDirectoryEntry(PageDirectoryPointerTable* pdpt, uint32_t virtualAddress, PageDirectoryEntry value) {
	PageDirectory* pd = (PageDirectory*)GetPageDirectoryPointerEntry(pdpt, virtualAddress);
	(*pd) [GetPageTableIndexFromAddress(virtualAddress)] = value;
}

PageDirectoryEntry GetPageDirectoryEntry(PageDirectoryPointerTable* pdpt, uint32_t virtualAddress) {
	PageDirectory* pd = (PageDirectory*)GetPageDirectoryPointerEntry(pdpt, virtualAddress);
	return (*pd) [GetPageTableIndexFromAddress(virtualAddress)];
}


void SetPageTableEntry(PageDirectoryPointerTable* pdpt, uint32_t virtualAddress, PageTableEntry value) {
	PageDirectoryEntry pd = GetPageDirectoryEntry(pdpt, virtualAddress);
	PageTable* pt = reinterpret_cast<PageTable*>(pd & (~0xfff));
	(*pt)[GetPageIndexFromAddress(virtualAddress)] = value;
}

PageTableEntry GetPageTableEntry(PageDirectoryPointerTable* pdpt, uint32_t virtualAddress) {
	PageDirectoryEntry pd = GetPageDirectoryEntry(pdpt, virtualAddress);
	PageTable* pt = reinterpret_cast<PageTable*>(pd & (~0xfff));
	return (*pt)[GetPageIndexFromAddress(virtualAddress)] ;
}


PageDirectoryEntry MakePageDirectoryEntry(uint64_t physicalAddress) {
	return physicalAddress & (~0xfff);
}

PageTableEntry MakePageTableEntry(uint64_t physicalAddress) {
	return physicalAddress & (~0xfff);
}


bool GetPageDirectoryFlag(PageDirectoryEntry entry, PageDirectoryFlag flag) {
	return entry & ((uint16_t)flag);
}

bool GetPageTableFlag(PageTableEntry entry, PageTableFlag flag) {
	return entry & ((uint16_t)flag);
}

PageDirectoryEntry SetPageDirectoryFlag(PageDirectoryEntry entry, PageDirectoryFlag flag, bool value) {
	if (value) {
		return entry | ((uint16_t)flag);
	} else {
		return entry & (~((uint16_t)flag));
	}
}

PageTableEntry SetPageTableFlag(PageTableEntry entry, PageTableFlag flag, bool value) {
	if (value) {
		return entry | ((uint16_t)flag);
	} else {
		return entry & (~((uint16_t)flag));
	}
}

}
}