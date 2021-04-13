.data
pOS_main_stack: .word 0

.thumb
.syntax unified
.cpu cortex-m0plus
.section .text, "ax"
.balign 4
.eabi_attribute Tag_ABI_align_preserved, 1

.global isr_systick
.global pOS_kernel_start
.global pOS_disable_and_save_interrupts
.global pOS_enable_and_restore_interrupts
.global pOS_disable_interrupts
.global pOS_enable_interrupts
.extern pOS_scheduler_tick
.extern pOS_stack_ptr

.thumb_func
isr_systick:					@R0-R3, R12, LR, PC, xPSR already pushed by interrupt
	CPSID IF

	PUSH {R4-R7}				@Store low registers
	MOV R4, R8
	MOV R5, R9
	MOV R6, R10
	MOV R7, R11
	PUSH {R4-R7}				@Store high registers

	LDR R0, =pOS_stack_ptr		@Save the stack
	LDR R2, [R0]
	MOV R1, SP
	STR R1, [R2]

	LDR R0, =pOS_main_stack		@Switch to main stack
	LDR R1, [R0]
	MOV SP, R1

	PUSH {LR}

	BL pOS_scheduler_tick		@Handle scheduler

	POP {R0}
	MOV LR, R0

	LDR R0, =pOS_stack_ptr		@Switch to new stack
	LDR R1, [R0]
	LDR R1, [R1]
	MOV SP, R1

	POP {R4-R7}					@Pop high registers
	MOV R8, R4
	MOV R9, R5
	MOV R10, R6
	MOV R11, R7
	POP {R4-R7}					@Pop low registers

	CPSIE IF
	BX LR						@Exit back out of interrupt

pOS_kernel_start:
	CPSID IF

	LDR R0, =pOS_main_stack		@Store the main stack (easy to use later)
	MOV R1, SP
	STR R1, [R0]

	LDR R0, =pOS_stack_ptr		@Get the new stack
	LDR R1, [R0]
	LDR R1, [R1]
	MOV SP, R1

	POP {R0}					@Get the argument of thread 1
	ADD SP, SP, #52				@Skip all the other registers (4 bytes per register, R1-R12 and LR)
	POP {R2}					@Pop the PC into R2
	ADD SP, SP, #4				@Move stack up to set it at the PSR

	PUSH {R0-R1}				@Store R0 temporarily

	LDR R0, =0xE000E014			@SYST_RVR register
	LDR R1, =0x1E847			@Setup SysTick reload value (System clock - 1)
	STR R1, [R0]
	LDR R0, =0xE000E010			@SYST_CSR
	MOVS R1, #0x7				@Enable SysTick and interrupt
	STR R1, [R0]

	POP {R0-R1}					@Get value back

	CPSIE IF
	BX R2						@Jump to the new function

.thumb_func
pOS_disable_and_save_interrupts:
	MRS R0, PRIMASK
	CPSID IF
	BX LR

.thumb_func
pOS_enable_and_restore_interrupts:
	MSR PRIMASK, R0
	CPSIE IF
	BX LR

.thumb_func
pOS_disable_interrupts:
	CPSID IF
	BX LR

.thumb_func
pOS_enable_interrupts:
	CPSIE IF
	BX LR

.align
.end