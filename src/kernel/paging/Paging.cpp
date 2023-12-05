/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "kernel/multiboot/Multiboot.h"
#include "Paging.h"
#include "MemoryLayout.h"
#include "BootDebug.h"
#include "PageTable.h"




namespace Kernel {
	
using namespace PageTable;
	
void Paging::bootstrapPaging(PageDirectoryPointerTable * pdpt, PageDirectory (*directory)[4], uint32_t *biosDirectory) {
	PageDirectoryEntry p1, p2; //two entries necessary for a 4MB page
	
    // Calculate 4 MiB page where (higher-half) kernel should start
    uint32_t kernelPage = MemoryLayout::KERNEL_START >> 22U;
    // Size of a 4 MiB page
    uint32_t bigPageSize = PAGESIZE * 1024U;
	uint32_t paePageSize = bigPageSize/2; //PAE big pages are only 2MB large

    auto *blockMap = reinterpret_cast<Multiboot::MemoryBlock*>(MemoryLayout::VIRTUAL_TO_PHYSICAL(reinterpret_cast<uint32_t>(Multiboot::getBlockMap())));
	
	//initialize page directory pointer table with addresses of the page directories
	for (uint8_t i=0; i<4;i++) {
		SetPageDirectoryPointerEntry(pdpt, i<<30, (uint64_t)&((*directory)[i]));
	}
	
	//Create paging and bios directory 
    uint32_t pageCount = 0;
    uint32_t blockMapIndex;
    for (blockMapIndex = 0; blockMap[blockMapIndex].blockCount != 0; blockMapIndex++) {
        if (!blockMap[blockMapIndex].initialMap) {
            continue;
        }

        uint32_t startAddress = blockMap[blockMapIndex].startAddress;
        blockMap[blockMapIndex].virtualStartAddress = (kernelPage + pageCount) * bigPageSize;

        for (uint32_t j = 0; j < blockMap[blockMapIndex].blockCount; j++) {
			p1 = MakePageDirectoryEntry(startAddress + j * bigPageSize);
			p2 = MakePageDirectoryEntry(startAddress + j * bigPageSize + paePageSize);
			
			p1 = SetPageDirectoryFlag(p1, PageDirectoryFlag::PRESENT, true);
			p1 = SetPageDirectoryFlag(p1, PageDirectoryFlag::READ_WRITE, true);
			p1 = SetPageDirectoryFlag(p1, PageDirectoryFlag::PAGE_SIZE, true);
			p2 = SetPageDirectoryFlag(p2, PageDirectoryFlag::PRESENT, true);
			p2 = SetPageDirectoryFlag(p2, PageDirectoryFlag::READ_WRITE, true);
			p2 = SetPageDirectoryFlag(p2, PageDirectoryFlag::PAGE_SIZE, true);
			
			//Map to both kernel space and user space
			SetPageDirectoryEntry(pdpt, pageCount * bigPageSize, p1);
			SetPageDirectoryEntry(pdpt, pageCount * bigPageSize + paePageSize, p2);
		
			SetPageDirectoryEntry(pdpt, (kernelPage + pageCount ) * bigPageSize, p1);
			SetPageDirectoryEntry(pdpt, (kernelPage + pageCount) * bigPageSize + paePageSize, p2);
			
			
            biosDirectory[pageCount] = (uint32_t) ((startAddress + j * bigPageSize) | PRESENT | READ_WRITE | PAGE_SIZE_MIB);
            biosDirectory[kernelPage + pageCount] = (uint32_t) ((startAddress + j * bigPageSize) | PRESENT | READ_WRITE | PAGE_SIZE_MIB);

            pageCount++;
        }
    }
		

    // Search 2 free blocks (4 MiB each) for initial heap and paging area
    uint32_t blocksFound = 0;
    uint32_t heapPhysicalAddress = 0;
    uint32_t pagingAreaPhysicalAddress = 0;
    for (uint32_t i = 0; blocksFound < 2; i++) {
        uint32_t blockSize = blockMap[i].initialMap ? bigPageSize : PAGESIZE;
        uint32_t freeStartAddress = blockMap[i].startAddress + blockMap[i].blockCount * blockSize;
        uint32_t freeEndAddress = blockMap[i + 1].startAddress;

        freeStartAddress = freeStartAddress % bigPageSize == 0 ? freeStartAddress : (freeStartAddress / bigPageSize) * bigPageSize + bigPageSize;
        freeEndAddress = (freeEndAddress / bigPageSize) * bigPageSize;

        // Overflow
        if (blockMap[i + 1].blockCount != 0 && freeStartAddress > freeEndAddress) {
            continue;
        }

        if (freeEndAddress - freeStartAddress >= bigPageSize || blockMap[i + 1].blockCount == 0) {
            // Shift block map entries to make space for inserting a new block
            for (uint32_t j = blockMapIndex; j > i; j--) {
                blockMap[j + 1] = blockMap[j];
            }

            if (blocksFound == 0) {
                heapPhysicalAddress = freeStartAddress;
                blockMap[i + 1] = {freeStartAddress, (kernelPage + pageCount) * bigPageSize, 1, true, Multiboot::HEAP_RESERVED};
                i = -1;
            } else {
                pagingAreaPhysicalAddress = freeStartAddress;
                blockMap[i + 1] = {freeStartAddress, MemoryLayout::PAGING_AREA.startAddress, 1, true, Multiboot::PAGING_RESERVED};
            }

            blocksFound++;
        }
    }

    // The first page of the initial heap above the reserved memory is mapped to an offset of KERNEL_START
    // No identity mapping needed because the heap is only used when paging is already enabled
	p1 = MakePageDirectoryEntry(heapPhysicalAddress);
	p2 = MakePageDirectoryEntry(heapPhysicalAddress + paePageSize);
	
	p1 = SetPageDirectoryFlag(p1, PageDirectoryFlag::PRESENT, true);
	p1 = SetPageDirectoryFlag(p1, PageDirectoryFlag::READ_WRITE, true);
	p1 = SetPageDirectoryFlag(p1, PageDirectoryFlag::PAGE_SIZE, true);
	p2 = SetPageDirectoryFlag(p2, PageDirectoryFlag::PRESENT, true);
	p2 = SetPageDirectoryFlag(p2, PageDirectoryFlag::READ_WRITE, true);
	p2 = SetPageDirectoryFlag(p2, PageDirectoryFlag::PAGE_SIZE, true);
	
	SetPageDirectoryEntry(pdpt, (kernelPage + pageCount ) * bigPageSize, p1);
	SetPageDirectoryEntry(pdpt, (kernelPage + pageCount) * bigPageSize + paePageSize, p2);


    // Calculate index to first virtual address of paging area memory
    // These first 10 MiB of the paging area are needed to set up the final 4 KiB paging,
    // so map the first (phys.) 10 MiB after the initial 4 MiB-heap to this address
	for (uint8_t i=0; i<4;i++) {
		p1 = MakePageDirectoryEntry(pagingAreaPhysicalAddress + i*paePageSize);
		p1 = SetPageDirectoryFlag(p1, PageDirectoryFlag::PRESENT, true);
		p1 = SetPageDirectoryFlag(p1, PageDirectoryFlag::READ_WRITE, true);
		p1 = SetPageDirectoryFlag(p1, PageDirectoryFlag::PAGE_SIZE, true);
		SetPageDirectoryEntry(pdpt, MemoryLayout::PAGING_AREA.startAddress  + i*paePageSize, p1);
	}

}

}