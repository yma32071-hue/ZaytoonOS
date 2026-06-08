.section .note.GNU-stack,"",@progbits
.text
.global enter_user_mode
.type enter_user_mode, @function
enter_user_mode:
    mov    %rdi, %rax
    mov    %rsi, %rdx
    mov    $0x202, %rcx

    pushq $0x20
    pushq %rdx
    pushq %rcx
    pushq $0x18
    pushq %rax
    iretq

.size enter_user_mode, .-enter_user_mode
