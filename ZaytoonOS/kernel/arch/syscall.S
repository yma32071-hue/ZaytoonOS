.intel_syntax noprefix

.section .note.GNU-stack,"",@progbits
.section .text

.global syscall_entry
.type syscall_entry, @function
syscall_entry:
    # 1. Push general purpose 32-bit registers to build the stack frame
    push eax
    push ecx
    push edx
    push ebx
    push ebp
    push esi
    push edi

    # 2. Pass the current stack pointer (esp) as the argument to syscall_dispatch
    push esp
    call syscall_dispatch
    add esp, 4           # Clean up the argument from the stack

    # 3. Store the return value (eax) into a safe 32-bit scratch register
    mov esi, eax

    # 4. Pop registers back in absolute reverse order
    pop edi
    pop esi
    pop ebp
    pop ebx
    pop edx
    pop ecx
    pop eax

    # 5. Move the saved return value back to eax for the application to read
    mov eax, esi

    # 6. Universal 32-bit return to user space
    iret