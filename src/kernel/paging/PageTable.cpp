#include "PageTable.h"


//Functions to manage a 32bit PAE paging structure


namespace Kernel {

namespace PageTable {

//Functions for extracting the bit fields from the virtual address	
uint8_t GetPageDirectoryIndexFromAddress(uint32_t address) {
	return (address>>30)&0b11;
}

uint16_t GetPageTableIndexFromAddress(uint32_t address) {
	return (address>>21)&0x1ff;
}

uint16_t GetPageIndexFromAddress(uint32_t address) {
	return (address>>12)&0x1ff;
}


//Functions for accessing the page directory pointer table
void SetPageDirectoryPointerEntry(PageDirectoryPointerTable* pdpt, uint32_t virtualAddress, PageDirectoryPointerEntry value) {
	(*pdpt)[GetPageDirectoryIndexFromAddress(virtualAddress)] = value | 1; //automatically marked as present
}

PageDirectoryPointerEntry GetPageDirectoryPointerEntry(PageDirectoryPointerTable* pdpt, uint32_t virtualAddress) {
	return (*pdpt)[GetPageDirectoryIndexFromAddress(virtualAddress)] & (~1);
}


//Function for accessing the page directory
void SetPageDirectoryEntry(PageDirectoryPointerTable* pdpt, uint32_t virtualAddress, PageDirectoryEntry value) {
	PageDirectory* pd = (PageDirectory*)GetPageDirectoryPointerEntry(pdpt, virtualAddress);
	(*pd) [GetPageTableIndexFromAddress(virtualAddress)] = value;
}

PageDirectoryEntry GetPageDirectoryEntry(PageDirectoryPointerTable* pdpt, uint32_t virtualAddress) {
	PageDirectory* pd = (PageDirectory*)GetPageDirectoryPointerEntry(pdpt, virtualAddress);
	return (*pd) [GetPageTableIndexFromAddress(virtualAddress)];
}


//Function for accessing the page table
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


//Functions for setting up table entries
PageDirectoryEntry MakePageDirectoryEntry(uint64_t physicalAddress) {
	return physicalAddress & (~0xfff);
}

PageTableEntry MakePageTableEntry(uint64_t physicalAddress) {
	return physicalAddress & (~0xfff);
}


//Functions for accessing table flags
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