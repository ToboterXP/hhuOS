/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_STREAMNODE_H
#define HHUOS_STREAMNODE_H

#include <cstdint>

#include "MemoryNode.h"
#include "lib/util/base/String.h"
#include "lib/util/io/file/File.h"

namespace Util {
namespace Io {
class InputStream;
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Filesystem::Memory {

class StreamNode : public MemoryNode {

public:
    /**
     * Constructor.
     */
     StreamNode(const Util::String &name, Util::Io::OutputStream *outputStream, Util::Io::InputStream *inputStream);

    /**
     * Constructor.
     */
    StreamNode(const Util::String &name, Util::Io::OutputStream *outputStream);

    /**
     * Constructor.
     */
    StreamNode(const Util::String &name, Util::Io::InputStream *inputStream);

    /**
     * Copy Constructor.
     */
    StreamNode(const StreamNode &copy) = delete;

    /**
     * Assignment operator.
     */
    StreamNode& operator=(const StreamNode &other) = delete;

    /**
     * Destructor.
     */
    ~StreamNode() override;

    /**
     * Overriding function from Node.
     */
    Util::Io::File::Type getType() override;

    /**
     * Overriding function from Node.
     */
    uint64_t readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) override;

    /**
     * Overriding function from Node.
     */
    uint64_t writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) override;

private:

    Util::Io::OutputStream *outputStream;
    Util::Io::InputStream *inputStream;

};

}

#endif
