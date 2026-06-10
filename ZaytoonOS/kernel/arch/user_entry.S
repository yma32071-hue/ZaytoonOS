.section .note.GNU-stack,"",@progbits
.text
.global enter_user_mode
.type enter_user_mode, @function
enter_user_mode:
    # In 32-bit, arguments are on the stack:
    # 4(%esp) = user entry point
    # 8(%esp) = user stack pointer
    mov 4(%esp), %eax   # Get user entry point
    mov 8(%esp), %edx   # Get user stack pointer
    mov $0x202, %ecx    # Set EFLAGS (Interrupts enabled, reserved bit 1)

    # Push the 32-bit User Mode Stack Frame
    pushl $0x23         # User Data Segment selector (GDT index 4 | RPL 3)
    pushl %edx          # User Stack Pointer (ESP)
    pushl %ecx          # User EFLAGS
    pushl $0x1B         # User Code Segment selector (GDT index 3 | RPL 3)
    pushl %eax          # User Instruction Pointer (EIP)
    
    iret                # Execute 32-bit jump down to user mode

.size enter_user_mode, .-enter_user_mode