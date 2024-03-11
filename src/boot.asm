; Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
; Institute of Computer Science, Department Operating Systems
; Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

; This is the entry-point for the whole system. The switch to protected mode is already done.
; Original by Olaf Spinczyk, TU Dortmund
; Refactored and extended by Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner, HHU

%include "constants.asm"

; 254 GB maximale RAM-Groesse fuer die Seitentabelle
MAX_MEM: equ 254

; Multiboot header
MULTIBOOT_HEADER_MAGIC equ 0xe85250d6
MULTIBOOT_HEADER_ARCHITECTURE equ 0
MULTIBOOT_HEADER_LENGTH equ (boot - multiboot_header)
MULTIBOOT_HEADER_CHECKSUM equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_ARCHITECTURE + MULTIBOOT_HEADER_LENGTH)

; Export variables
global initial_kernel_stack
global gdt_descriptor
global gdt_bios_descriptor
extern multiboot_data
extern acpi_data
extern smbios_data

; Export functions
global boot
global on_paging_enabled
global _init
global _fini
global __cxa_pure_virtual

; Import functions
extern main
extern init_gdt
extern copy_multiboot_info
extern copy_acpi_tables
extern copy_smbios_tables
extern initialize_memory_block_map
extern initialize_system
extern finish_system
extern setup_idt
extern reprogram_pics
extern enable_bootstrap_paging
extern enable_interrupts

; Import linker symbols
extern ___KERNEL_DATA_START__
extern ___KERNEL_DATA_END__
extern ___BSS_START__
extern ___BSS_END__
extern ___INIT_ARRAY_START__
extern ___INIT_ARRAY_END__
extern ___FINI_ARRAY_START__
extern ___FINI_ARRAY_END__
extern ___TEXT_START__
extern ___TEXT_END__

; Calculate physical addresses for some labels
; Needed while paging is disabled, because functions are linked against high addresses
_clear_bss          equ (clear_bss - KERNEL_START)
___PHYS_BSS_START__ equ (___BSS_START__ - KERNEL_START)
___PHYS_BSS_END__   equ (___BSS_END__ - KERNEL_START)

section .text

multiboot_header:
    ; Header
    align 8
    dd MULTIBOOT_HEADER_MAGIC
    dd MULTIBOOT_HEADER_ARCHITECTURE
    dd MULTIBOOT_HEADER_LENGTH
    dd MULTIBOOT_HEADER_CHECKSUM

    ; Address tag
    align 8
    dw MULTIBOOT_TAG_ADDRESS
    dw MULTIBOOT_TAG_FLAG_OPTIONAL
    dd 24
    dd (multiboot_header - KERNEL_START)
    dd (___KERNEL_DATA_START__ - KERNEL_START)
    dd (___KERNEL_DATA_END__ - KERNEL_START)
    dd (___BSS_END__ - KERNEL_START)

    ; Entry address tag
    align 8
	;dw MULTIBOOT_TAG_EFI_AMD64_ENTRY_ADDRESS
    dw MULTIBOOT_TAG_ENTRY_ADDRESS
    dw MULTIBOOT_TAG_FLAG_OPTIONAL
    dd 12
    dd (boot - KERNEL_START)

    ; Information request tag (required)
    align 8
    dw MULTIBOOT_TAG_INFORMATION_REQUEST
    dw 0
    dd 24
    dd MULTIBOOT_REQUEST_BOOT_COMMAND_LINE
    dd MULTIBOOT_REQUEST_MODULE
    dd MULTIBOOT_REQUEST_MEMORY_MAP
    dd MULTIBOOT_REQUEST_FRAMEBUFFER_INFO

    ; Information request tag (optional)
    align 8
    dw MULTIBOOT_TAG_INFORMATION_REQUEST
    dw MULTIBOOT_TAG_FLAG_OPTIONAL
    dd 24
    dd MULTIBOOT_REQUEST_BOOT_LOADER_NAME
    dd MULTIBOOT_REQUEST_SMBIOS_TABLES
    dd MULTIBOOT_REQUEST_ACPI_OLD_RSDP
    dd MULTIBOOT_REQUEST_ACPI_NEW_RSDP

    ; Framebuffer tag
    align 8
    dw MULTIBOOT_TAG_FRAMEBUFFER
    dw MULTIBOOT_TAG_FLAG_OPTIONAL
    dd 20
    dd GRAPHICS_WIDTH
    dd GRAPHICS_HEIGHT
    dd GRAPHICS_BPP

    ; Module alignment tag
    align 8
    dw MULTIBOOT_TAG_MODULE_ALIGNMENT
    dw MULTIBOOT_TAG_FLAG_OPTIONAL
    dd 8

    ; Termination tag
    align 8
    dw MULTIBOOT_TAG_TERMINATE
    dw 0
    dd 8

[BITS 32]
boot:

	cli 
	hlt

    ; Save multiboot structure address
    mov [multiboot_physical_addr - KERNEL_START], ebx

    ; Set esp to initial kernel stack
    mov esp, (initial_kernel_stack - KERNEL_START + STACK_SIZE)
	
	
	
    ; Setup global descriptor tables
    ;push dword (gdt_phys_descriptor - KERNEL_START)
    ;push dword (gdt_bios_descriptor - KERNEL_START)
    ;push dword (gdt_descriptor - KERNEL_START)
    ;push dword (gdt_bios - KERNEL_START)
    ;push dword (gdt - KERNEL_START)
	
	 ;call init_gdt
	
	
    ; Load GDT from physical address
    ;lgdt [gdt_phys_descriptor - KERNEL_START]
	
	
	lgdt   [_gdt_80 - KERNEL_START]  ; Neue Segmentdeskriptoren setzen
		
	mov    eax, 0x10     ; 3. Eintrag in der GDT
	  mov    ds, ax
	  
	 
	  
	  mov    es, ax
	  mov    fs, ax
	  mov    gs, ax

   ; Stack festlegen
	  mov    ss, ax
	  
	  
   
	jmp    0x28:(_init_longmode - KERNEL_START)
    ; Invoke a jump to set the CS-register to the right value (the code segment is placed at offset 0x8)
    ; If something at the GDT is changed, this instruction may need to be changed as well
    ;jmp 0x8:_clear_bss
	
	
	
; Zero out bss

	
;	jmp    _init_longmode
;
;  Umschalten in den 64 Bit Long-Mode
;
_init_longmode:


	 mov ebx, cr0 
	 and ebx, ~(1 << 31)
	 mov cr0, ebx
	 
		

	  ; Adresserweiterung (PAE) aktivieren
	  mov    eax, cr4
	  or     eax, 1 << 5
	  mov    cr4, eax
	  
	  

	  ; Seitentabelle anlegen (Ohne geht es nicht)
	  call   _setup_paging
	  
	 

	  ; Long-Mode (fürs erste noch im Compatibility-Mode) aktivieren
	  mov    ecx, 0x0C0000080 ; EFER (Extended Feature Enable Register) auswaehlen
	  rdmsr
	  or     eax, 1 << 8 ; LME (Long Mode Enable)
	  wrmsr
	  
	
	mov byte al, 0x41
	  out 0xe9, al
	
	  cli 
	  hlt
	  
	  ; Paging aktivieren
	  mov    eax, cr0
	  or     eax, 1 << 31
	  mov    cr0, eax
	  
	    
	  
	  
	  
	  mov    eax, 0x10     ; 3. Eintrag in der GDT
	  mov    ds, ax
	  
	 
	  
	  mov    es, ax
	  mov    fs, ax
	  mov    gs, ax

   ; Stack festlegen
	  mov    ss, ax
	  
	  
	  ; Sprung ins 64 Bit-Codesegment -> Long-Mode wird vollständig aktiviert
	  jmp    0x8: _longmode_start    ; CS = 2. Eintrag in der GDT
	
	
;
;   Anlegen einer (provisorischen) Seitentabelle mit 2 MB Seitengröße, die die
;   ersten MAX_MEM GB direkt auf den physikalischen Speicher abbildet.
;   Dies ist notwendig, da eine funktionierende Seitentabelle für den Long-Mode
;   vorausgesetzt wird. Mehr Speicher darf das System im Moment nicht haben.
;
_setup_paging:
	

   ; PML4 (Page Map Level 4 / 1. Stufe)
	  mov    eax, (_pdp - KERNEL_START)
	  or     eax, 0xf
	  mov    dword [_pml4 - KERNEL_START + 0], eax
	  mov    dword [_pml4 - KERNEL_START + 4], 0

	  ; PDPE (Page-Directory-Pointer Entry / 2. Stufe) für aktuell 16GB
	  mov    eax, _pd - KERNEL_START
	  or     eax, 0x7           ; Adresse der ersten Tabelle (3. Stufe) mit Flags.
	  mov    ecx, 0			; map to 0xc0000000
_fill_tables2:
	  cmp    ecx, MAX_MEM       ; MAX_MEM Tabellen referenzieren
	  je     _fill_tables2_done
	  mov    dword [_pdp - KERNEL_START + 8*ecx + 0], eax
	  mov    dword [_pdp - KERNEL_START + 8*ecx + 4], 0
	  add    eax, 0x1000        ; Die Tabellen sind je 4kB groß
	  inc    ecx
	  ja     _fill_tables2
_fill_tables2_done:

	; identity map first gb 
	mov    eax, _pd - KERNEL_START
	  or     eax, 0x7          ; Adresse der ersten Tabelle (3. Stufe) mit Flags.
	  ;mov    dword [_pdp - KERNEL_START ], eax
	  ;mov    dword [_pdp - KERNEL_START+ 4], 0

	  ; PDE (Page Directory Entry / 3. Stufe)
	  mov    eax, 0x0 | 0x87    ; Startadressenbyte 0..3 (=0) + Flags
	  mov    ebx, 0             ; Startadressenbyte 4..7 (=0)
	  mov    ecx, 0
_fill_tables3:
	  cmp    ecx, 512*MAX_MEM   ; MAX_MEM Tabellen mit je 512 Einträgen füllen
	  je     _fill_tables3_done
	  mov    dword [_pd - KERNEL_START + 8*ecx + 0], eax ; low bytes
	  mov    dword [_pd - KERNEL_START + 8*ecx + 4], ebx ; high bytes
	  add    eax, 0x200000      ; 2 MB je Seite
	  adc    ebx, 0             ; Overflow? -> Hohen Adressteil inkrementieren
	  inc    ecx
	  ja     _fill_tables3
_fill_tables3_done:

   ; Basiszeiger auf PML4 setzen
	  mov    eax, _pml4
	  mov    cr3, eax
	  ret
	
	
[BITS 64]
_longmode_start:

		
	
clear_bss:
    mov    edi,___PHYS_BSS_START__
clear_bss_loop:
    cmp    edi,___PHYS_BSS_END__
    jge clear_bss_done
    mov    byte [edi],0
    inc    edi
    jmp clear_bss_loop
clear_bss_done:
    ; Set stack again to cut off possible old values
    mov esp, (initial_kernel_stack - KERNEL_START + STACK_SIZE)

    ; Copy the multiboot info struct into bss
    push qword MULTIBOOT_SIZE
    push qword (multiboot_data - KERNEL_START)
    push qword [multiboot_physical_addr - KERNEL_START]
    call copy_multiboot_info

    ; Copy the ACPI structures into bss
   push qword ACPI_SIZE
   push qword (acpi_data - KERNEL_START)
   push qword [multiboot_physical_addr - KERNEL_START]
   call copy_acpi_tables

    ; Copy the SMBIOS structures into bss
    push qword SMBIOS_SIZE
   push qword (smbios_data - KERNEL_START)
    push qword [multiboot_physical_addr - KERNEL_START]
    call copy_smbios_tables

    ; Read memory map from multiboot info struct
    push qword [multiboot_physical_addr - KERNEL_START]
    call initialize_memory_block_map
	
	
	
	cli
	 hlt
	
	;set gdt to virtual memory
	;lgdt [gdt_descriptor]

    ; Set esp to initial kernel stack
    ;mov esp, (initial_kernel_stack + STACK_SIZE)
	

    ; Setup interrupts (see interrupts.asm)
    call setup_idt
	
	cli 
	hlt
	
	
    ;call reprogram_pics

    ; Initialize system
    ;call initialize_system

    ; Call the kernel's main() function
    ;call main

    ; Finalize system
    ;call finish_system
    hlt

; Call constructors of global objects
_init:
	mov eax, 0x1234501
	cli
	hlt
;    mov edi,___INIT_ARRAY_START__
;_init_loop:
;    cmp edi,___INIT_ARRAY_END__
;    jge _init_done
;    call [edi]
;    add    edi,4
;    jmp _init_loop
;_init_done:
;    ret

; Call destructors of global objects
_fini:
mov eax, 0x1234502
cli
	hlt
;    mov     edi,___FINI_ARRAY_START__
;_fini_loop:
;    cmp     edi,___FINI_ARRAY_END__
;    jge _fini_done
;    call [edi]
;    add     edi,4
;    jmp _fini_loop
;_fini_done:
;    ret

; This function is used when global constructors are called
; The label must be defined but can be void
__cxa_pure_virtual:
mov eax, 0x1234503
cli
	hlt
;    ret


[BITS 64]
section .data

; The following lines reserve memory for the different GDTs needed in hhuOS
; These GDTs and descriptors are set up in init_gdt (SystemManagement.cpp)

_gdt:
	  dw  0,0,0,0   ; NULL-Deskriptor

	  ; Kernel 64-Bit-Codesegment-Deskriptor
  	dw  0xFFFF    ; limit [00:15] = 4Gb - (0x100000*0x1000 = 4Gb)
	  dw  0x0000    ; base  [00:15] = 0
	  dw  0x9A00    ; base  [16:23] = 0, code read/exec, DPL=0, present
  	dw  0x00AF    ; limit [16:19], granularity=4096, 386, Long-Mode, base [24:31]

	  ; Kernel 64-Bit-Datensegment-Deskriptor 
	  dw  0xFFFF    ; limit [00:15] = 4Gb - (0x100000*0x1000 = 4Gb)
	  dw  0x0000    ; base  [00:15] = 0
	  dw  0x9200    ; base  [16:23] = 0, data read/write, DPL=0, present 
	  dw  0x00CF    ; limit [16:19], granularity=4096, 386, base [24:31]
	
	; User Moder 64-Bit Code Deskriptor
		dw  0xFFFF    ; limit [00:15] = 4Gb - (0x100000*0x1000 = 4Gb)
	  dw  0x0000    ; base  [00:15] = 0
	  dw  0xFA00    ; base  [16:23] = 0, code read/exec, DPL=3 present
  	dw  0x00AF    ; limit [16:19], granularity=4096, 386, Long-Mode, base [24:31]

	  ; User Moder 64-Bit Daten Deskriptor
	  dw  0xFFFF    ; limit [00:15] = 4Gb - (0x100000*0x1000 = 4Gb)
	  dw  0x0000    ; base  [00:15] = 0
	  dw  0xF200    ; base  [16:23] = 0, data read/write, DPL=3, present 
	  dw  0x00CF    ; limit [16:19], granularity=4096, 386, base [24:31]
	  
	  
	  ; Kernel 32-Bit-Codesegment-Deskriptor (nur fuer das Booten benoetigt)
	  dw  0xFFFF    ; limit [00:15] = 4Gb - (0x100000*0x1000 = 4Gb)
	  dw  0x0000    ; base  [00:15] = 0
	  dw  0x9A00    ; base  [16:23] = 0, code read/exec, DPL=0, present
	  dw  0x00CF    ; limit [16:19], granularity=4096, 386, base [24:31]

_gdt_tssd:	
	  ;TSS Descriptor	
	  dd 0x0000006C ; segment size 108 bytes
	   dd 0x00008900 ;granularity=0, present, DPL=0, type=9,
	   dd 0x00000000 ; tss base 64:32 
	   dd 0x00000000 
	 
	  


_gdt_80:
   ; 4 Eintraege in der GDT
		dw  6*8 + 16 - 1   ; GDT Limit=32, 7 GDT Eintraege - 1
	  dd  _gdt - KERNEL_START      ; Adresse der GDT
	  
gdt_descriptor:
	dw 0
	dd 0
	
gdt_phys_descriptor:
	dw 0
	dd 0
	
gdt:
	times(8*8) db 0

; Global descriptor table for bios calls
gdt_bios:
    times (5 * 8) db 0


; Descriptor for bios call gdt
gdt_bios_descriptor:
    dw    0                           ; GDT limit
    dd    0                            ; physical BIOS-GDT address
	

multiboot_physical_addr:
    dd  0

section .bss

; Reserve space for initial kernel stack
align 32
initial_kernel_stack:
    resb STACK_SIZE

; Reserve space for a copy of the multiboot information
multiboot_data:
    resb MULTIBOOT_SIZE

; Reserve space for a copy of the ACPI tables
acpi_data:
    resb ACPI_SIZE

; Reserve space for a copy of the SMBIOS tables
smbios_data:
    resb SMBIOS_SIZE

;
; Speicher fuer Page-Tables
;
[SECTION .global_pagetable]

[GLOBAL _pml4]
[GLOBAL _pdp]
[GLOBAL _pd]

_pml4:
   times 4096 db 0
	  alignb 4096

_pd:
   times MAX_MEM*4096 db 0
	  alignb 4096

_pdp:
   times MAX_MEM*8 db 0    ; 254*8 = 2032
   