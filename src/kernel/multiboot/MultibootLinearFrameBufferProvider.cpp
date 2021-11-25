#include <kernel/system/System.h>
#include <kernel/system/System.h>
#include <device/graphic/lfb/LinearFrameBufferNode.h>
#include "MultibootLinearFrameBufferProvider.h"
#include "Structure.h"

namespace Kernel::Multiboot {

MultibootLinearFrameBufferProvider::MultibootLinearFrameBufferProvider() : frameBufferInfo(Structure::getFrameBufferInfo()), supportedModes(1) {
    supportedModes[0] = {frameBufferInfo.width, frameBufferInfo.height, frameBufferInfo.bpp, frameBufferInfo.pitch, 0};
}

bool MultibootLinearFrameBufferProvider::isAvailable() {
    const auto &frameBufferInfo = Structure::getFrameBufferInfo();
    return frameBufferInfo.type == FRAMEBUFFER_TYPE_RGB && frameBufferInfo.bpp >= 15;
}

bool MultibootLinearFrameBufferProvider::initializeLinearFrameBuffer(const ModeInfo &modeInfo, const Util::Memory::String &filename) {
    if (!isAvailable()) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "LFB mode has not been setup correctly by the bootloader!");
    }

    // Create filesystem node
    auto &filesystem = Kernel::System::getService<Kernel::FilesystemService>().getFilesystem();
    auto &driver = filesystem.getVirtualDriver("/device");
    auto *lfbNode = new Device::Graphic::LinearFrameBufferNode(filename, frameBufferInfo.address, frameBufferInfo.width, frameBufferInfo.height,frameBufferInfo.bpp, frameBufferInfo.pitch);
    return driver.addNode("/", lfbNode);
}

Util::Data::Array<MultibootLinearFrameBufferProvider::ModeInfo> MultibootLinearFrameBufferProvider::getAvailableModes() const {
    return supportedModes;
}

Util::Memory::String MultibootLinearFrameBufferProvider::getClassName() const {
    return CLASS_NAME;
}

}