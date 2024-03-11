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

#ifndef __BITMAPMEMORYMANAGER_H__
#define __BITMAPMEMORYMANAGER_H__

#include <cstdint>

#include "lib/util/async/AtomicBitmap.h"
#include "BlockMemoryManager.h"

namespace Kernel {

/**
 * Memory manager, that manages a given area of memory in blocks of given size using a bitmap mechanism.
 *
 * @author Burak Akguel, Christian Gesse, Filip Krakowski, Fabian Ruhland, Michael Schoettner
 * @date 2018
 */
class BitmapMemoryManager : public BlockMemoryManager {

public:
    /**
     * Constructor.
     */
    BitmapMemoryManager(uint8_t *startAddress, uint8_t *endAddress, uint64_t blockSize = 4096, bool zeroMemory = false);

    /**
     * Copy Constructor.
     */
    BitmapMemoryManager(const BitmapMemoryManager &copy) = delete;

    /**
     * Assignment operator.
     */
    BitmapMemoryManager& operator=(const BitmapMemoryManager &other) = delete;

    /**
     * Destructor.
     */
    ~BitmapMemoryManager() override = default;

    [[nodiscard]] void *allocateBlock() override;

    void freeBlock(void *pointer) override;

    virtual void handleError();

    [[nodiscard]] uint64_t getTotalMemory() const override;

    [[nodiscard]] uint64_t getFreeMemory() const override;

    [[nodiscard]] uint64_t getBlockSize() const override;

    [[nodiscard]] uint8_t* getStartAddress() const override;

    [[nodiscard]] uint8_t* getEndAddress() const override;

protected:

    void setRange(uint64_t startBlock, uint64_t blockCount);

private:

    uint8_t *startAddress;
    uint8_t *endAddress;
    uint64_t freeMemory;

    uint64_t blockSize;
    bool zeroMemory = false;

    Util::Async::AtomicBitmap bitmap;

};

}

#endif
