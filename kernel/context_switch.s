.syntax unified
.thumb
.thumb_func


.global PendSV_Handler
PendSV_Handler:
@ Save the current context
    @mrs ip, psr
    push {r4-r11}
    
    mrs r0, psp
    ldr r1, =current_task
    str r0, [r1]

    bl Scheduler_Switch
    
    ldr r0, =current_task
    ldr r1, [r0]
    msr psp, r1

@ Load user state
	pop {r4, r5, r6, r7, r8, r9, r10, r11}
    @msr psr, ip

@ Jump to user task
	bx lr
    