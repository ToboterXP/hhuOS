; Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
; Institute of Computer Science, Department Operating Systems
; Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner; Olaf Spinczyk, TU Dortmund
;
; This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
; License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
; later version.
;
; This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
; warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
; details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>

%ifndef CONSTANTS_ASM
%define CONSTANTS_ASM

; Multiboot constants
MUTLIBOOT_EAX_MAGIC equ 0x36d76289

; Multiboot tag types
MULTIBOOT_TAG_TERMINATE equ 0
MULTIBOOT_TAG_INFORMATION_REQUEST equ 1
MULTIBOOT_TAG_ADDRESS equ 2
MULTIBOOT_TAG_ENTRY_ADDRESS equ 3
MULTIBOOT_TAG_FLAGS equ 4
MULTIBOOT_TAG_FRAMEBUFFER equ 5
MULTIBOOT_TAG_MODULE_ALIGNMENT equ 6
MULTIBOOT_TAG_EFI_BOOT_SERVICES equ 7
MULTIBOOT_TAG_EFI_I386_ENTRY_ADDRESS equ 8
MULTIBOOT_TAG_EFI_AMD64_ENTRY_ADDRESS equ 9
MULTIBOOT_TAG_RELOCATABLE_HEADER equ 10

; Multiboot request types
MULTIBOOT_REQUEST_BOOT_COMMAND_LINE equ 1
MULTIBOOT_REQUEST_BOOT_LOADER_NAME equ 2
MULTIBOOT_REQUEST_MODULE equ 3
MULTIBOOT_REQUEST_BASIC_MEMORY_INFORMATION equ 4
MULTIBOOT_REQUEST_BIOS_BOOT_DEVICE equ 5
MULTIBOOT_REQUEST_MEMORY_MAP equ 6
MULTIBOOT_REQUEST_VBE_INFO equ 7
MULTIBOOT_REQUEST_FRAMEBUFFER_INFO equ 8
MULTIBOOT_REQUEST_ELF_SYMBOLS equ 9
MULTIBOOT_REQUEST_APM_TABLE equ 10
MULTIBOOT_REQUEST_EFI_32_BIT_SYSTEM_TABLE_POINTER equ 11
MULTIBOOT_REQUEST_EFI_64_BIT_SYSTEM_TABLE_POINTER equ 12
MULTIBOOT_REQUEST_SMBIOS_TABLES equ 13
MULTIBOOT_REQUEST_ACPI_OLD_RSDP equ 14
MULTIBOOT_REQUEST_ACPI_NEW_RSDP equ 15
MULTIBOOT_REQUEST_NETWORKING_INFORMATION equ 16
MULTIBOOT_REQUEST_EFI_MEMORY_MAP equ 17
MULTIBOOT_REQUEST_EFI_BOOT_SERVICES_NOT_TERMINATED equ 18
MULTIBOOT_REQUEST_EFI_32_BIT_IMAGE_HANDLE_POINTER equ 19
MULTIBOOT_REQUEST_EFI_64_BIT_IMAGE_HANDLE_POINTER equ 20
MULTIBOOT_REQUEST_IMAGE_LOAD_BASE_PHYSICAL_ADDRESS equ 21

; Multiboot tag flags
MULTIBOOT_TAG_FLAG_OPTIONAL equ 0x01

; Multiboot console flags
MULTIBOOT_CONSOLE_FLAG_FORCE_TEXT_MODE equ 0x01
MULTIBOOT_CONSOLE_FLAG_SUPPORT_TEXT_MODE equ 0x02

; Multiboot framebuffer options
GRAPHICS_WIDTH equ 800
GRAPHICS_HEIGHT equ 600
GRAPHICS_BPP equ 32

; Kernel constants
KERNEL_START equ 0xc0000000
STACK_SIZE equ 0x00001000
MULTIBOOT_SIZE equ 0x00010000
ACPI_SIZE equ 0x00010000
SMBIOS_SIZE equ 0x00010000

; BIOS call constants
BIOS_CALL_CODE equ 0x00000500
BIOS_CALL_ESP_BACKUP equ 0x00000600
BIOS_CALL_IDT equ 0x00000604
BIOS_CALL_STACK equ 0x00000700

%endif