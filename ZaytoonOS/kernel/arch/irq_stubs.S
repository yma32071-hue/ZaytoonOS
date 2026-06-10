.intel_syntax noprefix

.section .note.GNU-stack,"",@progbits
.section .text

.global default_interrupt_stub
.type default_interrupt_stub, @function
default_interrupt_stub:
    # Push general-purpose 32-bit registers
    push eax
    push ebx
    push ecx
    push edx
    push esi
    push edi
    push ebp
    
    call default_interrupt_handler
    
    # Pop in absolute reverse order
    pop ebp
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    iret

.global timer_interrupt_stub
.type timer_interrupt_stub, @function
timer_interrupt_stub:
    # Push general-purpose 32-bit registers
    push eax
    push ebx
    push ecx
    push edx
    push esi
    push edi
    push ebp
    
    call timer_interrupt_handler
    
    # Pop in absolute reverse order
    pop ebp
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    iret