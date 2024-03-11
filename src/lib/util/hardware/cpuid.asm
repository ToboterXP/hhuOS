global is_cpuid_available

; Code taken from: https://wiki.osdev.org/CPUID
is_cpuid_available:
    pushfq                              ; Save EFLAGS
    pushfq                               ; Store EFLAGS
    xor dword [rsp],0x00200000           ; Invert the ID bit in stored EFLAGS
    popfq                                ; Load stored EFLAGS (with ID bit inverted)
    pushfq                               ; Store EFLAGS again (ID bit may or may not be inverted)
    pop rax                              ; eax = modified EFLAGS (ID bit may or may not be inverted)
    xor rax,[rsp]                        ; eax = whichever bits were changed
    popfq                                ; Restore original EFLAGS
    and rax,0x00200000                   ; eax = zero if ID bit can't be changed, else non-zero
    ret