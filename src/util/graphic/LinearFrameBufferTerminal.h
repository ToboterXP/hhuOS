/*
 * Copyright (C) 2021 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef HHUOS_LINEARFRAMEBUFFERTERMINAL_H
#define HHUOS_LINEARFRAMEBUFFERTERMINAL_H

#include "LinearFrameBuffer.h"
#include "Colors.h"
#include "PixelDrawer.h"
#include "StringDrawer.h"
#include "Fonts.h"
#include "BufferScroller.h"
#include "Terminal.h"

namespace Util::Graphic {

class LinearFrameBufferTerminal : public Terminal {

public:

    explicit LinearFrameBufferTerminal(LinearFrameBuffer &lfb, Font &font = Fonts::TERMINAL_FONT, char cursor = '_');

    LinearFrameBufferTerminal(const LinearFrameBufferTerminal &copy) = delete;

    LinearFrameBufferTerminal &operator=(const LinearFrameBufferTerminal &other) = delete;

    ~LinearFrameBufferTerminal() override = default;

    void putChar(char c) override;

    void setPosition(uint16_t column, uint16_t row) override;

    void clear() override;

    void setForegroundColor(Color &color) override;

    void setBackgroundColor(Color &color) override;

    [[nodiscard]] LinearFrameBuffer& getLinearFrameBuffer() const;

private:

    const char cursor;

    LinearFrameBuffer &lfb;
    BufferScroller scroller;
    PixelDrawer pixelDrawer;
    StringDrawer stringDrawer;

    Font &font;
    Color fgColor = Colors::TERM_WHITE;
    Color bgColor = Colors::TERM_BLACK;
    uint16_t currentColumn = 0;
    uint16_t currentRow = 0;
};

}

#endif
