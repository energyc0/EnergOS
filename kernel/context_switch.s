.thumb

.global context_switch
context_switch:
@ Save the current context
    msr ip, psr
    push {r4-r11, lr}

@ Load new context stack pointer
    msr psp, r0
@ Switch to unprivileged thread mode
    mov r0, #3
    msr control, r0

@ Load user state
	pop {r4, r5, r6, r7, r8, r9, r10, r11, lr}

@ Jump to user task
	bx lr