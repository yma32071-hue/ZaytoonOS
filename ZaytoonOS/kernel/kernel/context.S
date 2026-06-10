.intel_syntax noprefix

.section .note.GNU-stack,"",@progbits
.section .text

.global context_switch
.type context_switch, @function
context_switch:
    # 1. Fetch the arguments from the 32-bit stack frame
    # 4[esp]  = pointer to old task's context storage (where to save)
    # 8[esp]  = pointer to new task's context storage (where to restore from)
    mov eax, [esp + 4]    # eax = old_context
    mov edx, [esp + 8]    # edx = new_context

    # 2. Save the current CPU state into the old task structure
    mov [eax + 0], ebx
    mov [eax + 4], esi
    mov [eax + 8], edi
    mov [eax + 12], ebp
    mov [eax + 16], esp   # Saves old stack pointer

    # 3. Restore the new CPU state from the new task structure
    mov ebx, [edx + 0]
    mov esi, [edx + 4]
    mov edi, [edx + 8]
    mov ebp, [edx + 12]
    mov esp, [edx + 16]   # Switches to the new task's stack here

    # 4. Return into the new task's code execution path
    ret