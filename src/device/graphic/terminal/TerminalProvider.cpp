#include <lib/util/data/ArrayList.h>
#include "TerminalProvider.h"

namespace Device::Graphic {

TerminalProvider::ModeInfo TerminalProvider::searchMode(uint16_t columns, uint16_t rows, uint8_t colorDepth) const {
    auto modes = getAvailableModes();
    auto candidates = Util::Data::ArrayList<ModeInfo>();

    if (modes.length() == 0) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "LinearFrameBufferProvider: No graphics mode available!");
    }

    uint32_t bestDiff = 0xffffffff;
    ModeInfo bestMode{};

    // Find the closest vertical resolution;
    for (const ModeInfo &mode : modes) {
        uint32_t diff = mode.rows > rows ? mode.rows - rows : rows - mode.rows;

        if (diff < bestDiff) {
            bestDiff = diff;
            bestMode = mode;
        }
    }

    // Mark all modes with the closest vertical resolution as candidates
    for (const ModeInfo &mode : modes) {
        if (mode.rows == bestMode.rows) {
            candidates.add(mode);
        }
    }

    bestDiff = 0xffffffff;

    // Search candidates for the closest horizontal resolution
    for (const ModeInfo &mode : candidates) {
        uint32_t diff = mode.columns > columns ? mode.columns - columns : columns - mode.columns;

        if (diff < bestDiff) {
            bestDiff = diff;
            bestMode = mode;
        }
    }

    // Remove all candidates, that don't have the closest horizontal resolution
    auto removeList = Util::Data::ArrayList<ModeInfo>();
    for (const ModeInfo &mode : candidates) {
        if (mode.columns != bestMode.columns) {
            removeList.add(mode);
        }
    }
    candidates.removeAll(removeList);

    bestDiff = 0xffffffff;

    // Find the mode with closest color depth
    for (const ModeInfo &mode : candidates) {
        uint32_t diff = mode.colorDepth > colorDepth ? mode.colorDepth - colorDepth : colorDepth - mode.colorDepth;

        if (diff < bestDiff) {
            bestDiff = diff;
            bestMode = mode;
        }
    }

    return bestMode;
}
}