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

#ifndef HHUOS_TABLEMEMORYMANAGER_H
#define HHUOS_TABLEMEMORYMANAGER_H

#include <cstdint>

#include "lib/util/async/Atomic.h"
#include "kernel/memory/BlockMemoryManager.h"
#include "lib/util/base/Exception.h"

namespace Kernel {
class BitmapMemoryManager;
class Logger;

class TableMemoryManager : public BlockMemoryManager {

public:
    /**
     * Constructor.
     */
    TableMemoryManager(BitmapMemoryManager &bitmapMemoryManager, uint8_t *startAddress, uint8_t *endAddress, uint64_t blockSize = 4096);

    /**
     * Copy Constructor.
     */
    TableMemoryManager(const TableMemoryManager &copy) = delete;

    /**
     * Assignment operator.
     */
    TableMemoryManager& operator=(const TableMemoryManager &other) = delete;

    /**
     * Destructor.
     */
    ~TableMemoryManager() override = default;

    void setMemory(uint8_t *start, uint8_t *end, uint16_t useCount, bool reserved);

    [[nodiscard]] void* allocateBlock() override;

    [[nodiscard]] void* allocateBlockAtAddress(void *address);

    [[nodiscard]] void* allocateBlockAfterAddress(void *address);

    void freeBlock(void *pointer) override;

    [[nodiscard]] uint64_t getTotalMemory() const override;

    [[nodiscard]] uint64_t getBlockSize() const override;

    [[nodiscard]] uint64_t getFreeMemory() const override;

    [[nodiscard]] uint8_t* getStartAddress() const override;

    [[nodiscard]] uint8_t* getEndAddress() const override;

    void debugLog();

private:

    void printAllocationTable(uint64_t referenceTableArrayIndex, uint64_t referenceTableIndex);

    struct ReferenceTableEntry {

    private:
        /**
         * 0-1:    unused
         * 2:      lock
         * 3:      installed
         * 4-31:   address
         */
        uint64_t value;

    public:

        void setAddress(uint64_t address) {
            auto valueWrapper = Util::Async::Atomic<uint64_t>(value);
            uint64_t oldValue;
            uint64_t exchangeValue;

            do {
                oldValue = valueWrapper.get();
                exchangeValue = (oldValue & 0x0000000f) | (address & 0xfffffff0);
            } while (valueWrapper.getAndSet(exchangeValue) != oldValue);
        }

        void setInstalled(bool installed) {
            auto valueWrapper = Util::Async::Atomic<uint64_t>(value);
            if (installed) {
                valueWrapper.bitSet(3);
            } else {
                valueWrapper.bitReset(3);
            }
        }

        void acquireLock() {
            while (!tryAcquireLock());
        }

        bool tryAcquireLock() {
            auto valueWrapper = Util::Async::Atomic<uint64_t>(value);
            return !valueWrapper.bitTestAndSet(2);
        }

        void releaseLock() {
            auto valueWrapper = Util::Async::Atomic<uint64_t>(value);
            valueWrapper.bitReset(2);
        }

        [[nodiscard]] uint64_t getAddress() {
            auto valueWrapper = Util::Async::Atomic<uint64_t>(value);
            return valueWrapper.get() & 0xfffffff0;
        }

        [[nodiscard]] bool isInstalled() {
            auto valueWrapper = Util::Async::Atomic<uint64_t>(value);
            return valueWrapper.bitTest(3);
        }

        [[nodiscard]] bool isLocked() {
            auto valueWrapper = Util::Async::Atomic<uint64_t>(value);
            return valueWrapper.bitTest(2);
        }
    };

    struct AllocationTableEntry {

    private:
        /**
         * 0:       reserved
         * 1-15:    useCount
         */
        uint16_t value;

    public:

        void setReserved(bool reserved) {
            auto valueWrapper = Util::Async::Atomic<uint16_t>(value);
            if (reserved) {
                valueWrapper.bitSet(0);
            } else {
                valueWrapper.bitReset(0);
            }
        }

        void setUseCount(uint16_t useCount) {
            auto valueWrapper = Util::Async::Atomic<uint16_t>(value);
            uint16_t oldValue;
            uint16_t exchangeValue;

            do {
                oldValue = valueWrapper.get();
                exchangeValue = (oldValue & 0x0001) | (useCount << 1);
            } while (valueWrapper.getAndSet(exchangeValue) != oldValue);
        }

        void incrementUseCount() {
            auto valueWrapper = Util::Async::Atomic<uint16_t>(value);
            uint16_t oldValue = valueWrapper.fetchAndAdd(0b0000000000000010);
            if (static_cast<uint16_t>(oldValue + 2) <= oldValue) {
                Util::Exception::throwException(Util::Exception::Error::PAGING_ERROR, "Page frame has been mapped too often!");
            }
        }

        void decrementUseCount() {
            auto valueWrapper = Util::Async::Atomic<uint16_t>(value);
            uint16_t oldValue = valueWrapper.fetchAndSub(0b0000000000000010);
            if (static_cast<uint16_t>(oldValue - 2) >= oldValue) {
                Util::Exception::throwException(Util::Exception::Error::PAGING_ERROR, "Page frame underflow!");
            }
        }

        [[nodiscard]] uint16_t getUseCount() {
            auto valueWrapper = Util::Async::Atomic<uint16_t>(value);
            return valueWrapper.get() >> 1;
        }

        [[nodiscard]] bool isReserved() {
            auto valueWrapper = Util::Async::Atomic<uint16_t>(value);
            return valueWrapper.bitTest(0);
        }
    };

    struct TableIndex {
        uint64_t referenceTableArrayIndex;
        uint64_t referenceTableIndex;
        uint64_t allocationTableIndex;
    };

    [[nodiscard]] TableIndex calculateIndex(uint8_t *address) const;

    [[nodiscard]] uint64_t calculateAddress(const TableIndex &index) const;

private:

    BitmapMemoryManager &bitmapMemoryManager;
    uint8_t *startAddress;
    uint8_t *endAddress;
    uint64_t blockSize;

    uint64_t referenceTableSizeInBlocks;
    uint64_t allocationTableEntriesPerBlock;
    uint64_t managedMemoryPerAllocationTable;
    uint64_t referenceTableEntriesPerBlock;
    uint64_t allocationTableCount;

    ReferenceTableEntry **referenceTableArray;

    static Logger log;
    static const constexpr uint64_t MIN_BITMAP_BLOCK_SIZE = 16;
};

}

#endif
