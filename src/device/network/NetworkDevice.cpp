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

#include "NetworkDevice.h"

#include "kernel/system/System.h"
#include "kernel/service/ProcessService.h"
#include "lib/util/async/Thread.h"
#include "device/network/PacketReader.h"
#include "device/network/PacketWriter.h"
#include "kernel/process/Thread.h"
#include "kernel/service/MemoryService.h"
#include "kernel/service/SchedulerService.h"
#include "lib/util/base/Address.h"
#include "lib/util/base/Constants.h"
#include "kernel/network/ethernet/EthernetModule.h"

namespace Device::Network {

NetworkDevice::NetworkDevice() :
        packetMemory(static_cast<uint8_t*>(Kernel::System::getService<Kernel::MemoryService>().allocateKernelMemory(MAX_BUFFERED_PACKETS * PACKET_BUFFER_SIZE, Util::PAGESIZE))),
        packetMemoryManager(packetMemory, packetMemory + MAX_BUFFERED_PACKETS * PACKET_BUFFER_SIZE - 1, PACKET_BUFFER_SIZE),
        incomingPacketQueue(MAX_BUFFERED_PACKETS),
        outgoingPacketQueue(MAX_BUFFERED_PACKETS),
        reader(new PacketReader(*this)),
        writer(new PacketWriter(*this)),
        log(Kernel::Logger::get("Network")) {
    auto &schedulerService = Kernel::System::getService<Kernel::SchedulerService>();
    auto &processService = Kernel::System::getService<Kernel::ProcessService>();
    auto &readerThread = Kernel::Thread::createKernelThread(Util::String::format("Packet-Reader"), processService.getKernelProcess(), reader);
    auto &writerThread = Kernel::Thread::createKernelThread(Util::String::format("Packet-Writer"), processService.getKernelProcess(), writer);

    schedulerService.ready(readerThread);
    schedulerService.ready(writerThread);
}

const Util::String& NetworkDevice::getIdentifier() const {
    return identifier;
}

void NetworkDevice::sendPacket(const uint8_t *packet, uint32_t length) {
    outgoingPacketLock.acquire();
    auto *buffer = reinterpret_cast<uint8_t*>(packetMemoryManager.allocateBlock());
    auto source = Util::Address<uint64_t>(packet);
    auto target = Util::Address<uint64_t>(buffer);
    target.copyRange(source, length);

    outgoingPacketQueue.add(Packet{buffer, length});
    outgoingPacketLock.release();
}

void NetworkDevice::handleIncomingPacket(const uint8_t *packet, uint32_t length) {
    if (!Kernel::Network::Ethernet::EthernetModule::checkPacket(packet, length)) {
        return;
    }

    auto *buffer = reinterpret_cast<uint8_t*>(packetMemoryManager.allocateBlock());
    auto source = Util::Address<uint64_t>(packet);
    auto target = Util::Address<uint64_t>(buffer);
    target.copyRange(source, length);

    if (!incomingPacketQueue.offer(Packet{buffer, length})) {
        packetMemoryManager.freeBlock(buffer);
    }
}

NetworkDevice::~NetworkDevice() {
    delete[] packetMemory;
}

NetworkDevice::Packet NetworkDevice::getNextIncomingPacket() {
    while (incomingPacketQueue.isEmpty()) {
        Util::Async::Thread::yield();
    }

    return incomingPacketQueue.poll();
}

NetworkDevice::Packet NetworkDevice::getNextOutgoingPacket() {
    while (outgoingPacketQueue.isEmpty()) {
        Util::Async::Thread::yield();
    }

    return outgoingPacketQueue.poll();
}

void NetworkDevice::freePacketBuffer(void *buffer) {
    packetMemoryManager.freeBlock(buffer);
}

void NetworkDevice::freeLastSendBuffer() {
    writer->freeLastSendBuffer();
}

bool NetworkDevice::Packet::operator==(const NetworkDevice::Packet &other) const {
    return buffer == other.buffer;
}

}