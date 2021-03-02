/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#include <cstdint>
#include <device/cpu/Cpu.h>
#include <util/stream/TerminalOutputStream.h>
#include <util/stream/StringFormatOutputStream.h>
#include <util/stream/BufferedOutputStream.h>
#include <device/bios/Bios.h>
#include <device/graphic/VesaBiosExtensions.h>
#include <kernel/multiboot/MultibootLinearFrameBufferProvider.h>
#include <device/graphic/LinearFrameBufferTerminalProvider.h>
#include <device/graphic/ColorGraphicsArrayProvider.h>
#include <util/reflection/InstanceFactory.h>
#include <kernel/multiboot/Structure.h>
#include "GatesOfHell.h"
#include "BuildConfig.h"

int32_t main() {
    GatesOfHell::enter();
}

void GatesOfHell::enter() {
    if (Device::Bios::isAvailable()) {
        Device::Bios::init();
    }

    if (Device::Graphic::VesaBiosExtensions::isAvailable()) {
        Util::Reflection::InstanceFactory::registerPrototype(new Device::Graphic::VesaBiosExtensions(true));
    }

    if (Device::Graphic::ColorGraphicsArrayProvider::isAvailable()) {
        Util::Reflection::InstanceFactory::registerPrototype(new Device::Graphic::ColorGraphicsArrayProvider(true));
    }

    Device::Graphic::LinearFrameBufferProvider *lfbProvider;
    Device::Graphic::TerminalProvider *terminalProvider;

    if (Kernel::Multiboot::Structure::hasKernelOption("lfb_provider")) {
        auto providerName = Kernel::Multiboot::Structure::getKernelOption("lfb_provider");
        lfbProvider = reinterpret_cast<Device::Graphic::LinearFrameBufferProvider*>(Util::Reflection::InstanceFactory::createInstance(providerName));
    } else {
        lfbProvider = new Kernel::Multiboot::MultibootLinearFrameBufferProvider();
    }

    if (Kernel::Multiboot::Structure::hasKernelOption("terminal_provider")) {
        auto providerName = Kernel::Multiboot::Structure::getKernelOption("terminal_provider");
        terminalProvider = reinterpret_cast<Device::Graphic::TerminalProvider*>(Util::Reflection::InstanceFactory::createInstance(providerName));
    } else {
        terminalProvider = new Device::Graphic::LinearFrameBufferTerminalProvider(*lfbProvider);
    }

    auto resolution = terminalProvider->searchMode(100, 37, 24);
    auto &terminal = terminalProvider->initializeTerminal(resolution);
    auto terminalStream = Util::Stream::TerminalOutputStream(terminal);
    auto bufferedStream = Util::Stream::BufferedOutputStream(terminalStream);
    auto outputStream = Util::Stream::StringFormatOutputStream(bufferedStream);

    outputStream << "Welcome to hhuOS!" << Util::Stream::StringFormatOutputStream::endl
                 << "Version: " << BuildConfig::getVersion() << " (" << BuildConfig::getGitBranch() << ")"
                 << Util::Stream::StringFormatOutputStream::endl
                 << "Git revision: " << BuildConfig::getGitRevision() << Util::Stream::StringFormatOutputStream::endl
                 << "Build date: " << BuildConfig::getBuildDate() << Util::Stream::StringFormatOutputStream::endl;

    Device::Cpu::halt();
}