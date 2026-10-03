.syntax unified
.thumb

.extern SVCall_Handler_C

.thumb_func
.global PendSV_Handler
PendSV_Handler:
    mrs r0, psp
@ Save the current context
    stmdb r0!, {r4-r11, lr}
    
    ldr r2, =current_task
    ldr r1, [r2]
    str r0, [r1]

    bl Scheduler_Switch

    ldr r2, =current_task
    ldr r1, [r2]
    ldr r0, [r1]

@ Load user state
	ldmia r0!, {r4-r11, lr}

    msr psp, r0
@ Jump to user task
	bx lr

.thumb_func
.global SVCall_Handler
SVCall_Handler:
    tst lr, #4
    ite eq
    mrseq r0, msp
    mrsne r0, psp
    b SVCall_Handler_C