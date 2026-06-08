.intel_syntax noprefix

.section .note.GNU-stack,"",@progbits
.section .text

.global syscall_entry
.type syscall_entry, @function
syscall_entry:
    swapgs
    push r11
    push rcx
    push rax
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10

    mov rdi, rsp
    call syscall_dispatch
    mov r12, rax

    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    pop rdx
    pop rax
    pop rcx
    pop r11
    mov rax, r12
    swapgs
    sysretq
