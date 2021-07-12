/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_LOGGER_H
#define HHUOS_LOGGER_H

#include <lib/util/memory/String.h>
#include <lib/util/async/Spinlock.h>
#include <lib/util/data/ArrayList.h>
#include <lib/util/stream/OutputStream.h>
#include <lib/util/data/HashMap.h>
#include <device/port/serial/SerialPort.h>
#include <device/port/serial/SerialOutputStream.h>
#include <lib/util/stream/PrintWriter.h>

namespace Kernel {

class Logger {

public:

    enum LogLevel {
        TRACE = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4
    };

    /**
     * Destructor.
     */
    ~Logger() = default;

    /**
     * Assignment operator.
     */
    Logger &operator=(const Logger &other) = delete;

    static Logger get(const Util::Memory::String &name);

    static void setLevel(LogLevel level);

    static void setLevel(Util::Memory::String level);

    static void addOutputStream(Util::Stream::OutputStream &stream);

    static void removeOutputStream(Util::Stream::OutputStream &stream);

    void trace(const Util::Memory::String &message, ...);

    void debug(const Util::Memory::String &message, ...);

    void info(const Util::Memory::String &message, ...);

    void warn(const Util::Memory::String &message, ...);

    void error(const Util::Memory::String &message, ...);

private:

    explicit Logger(const Util::Memory::String &name);

    /**
     * Copy constructor.
     */
    Logger(const Logger &other) = default;

    static const char* getLevelAsString(const LogLevel &level);

    static void logMessage(const LogLevel &level, const Util::Memory::String &name, const Util::Memory::String &message);

    const Util::Memory::String name;

    static Util::Async::Spinlock lock;
    static Util::Data::HashMap<Util::Stream::OutputStream*, Util::Stream::PrintWriter*> writerMap;
    static Util::Data::ArrayList<Util::Memory::String> buffer;
    static LogLevel currentLevel;

    static constexpr const char *LEVEL_TRACE = "TRACE";
    static constexpr const char *LEVEL_DEBUG = "DEBUG";
    static constexpr const char *LEVEL_INFO = "INFO";
    static constexpr const char *LEVEL_WARN = "WARN";
    static constexpr const char *LEVEL_ERROR = "ERROR";
};

}

#endif