        ; 8080 assembler code
        .hexfile shell.hex
        .binfile shell.com
        ; try "hex" for downloading in hex format
        .download bin  
        .objcopy gobjcopy
        .postbuild echo "OK!"
        ;.nodump

	; OS call list
PRINT_B		equ 1
PRINT_MEM	equ 2
READ_B		equ 3
READ_MEM	equ 4
PRINT_STR	equ 5
READ_STR	equ 6
FORK		equ 7
EXEC		equ 8
WAIT_PID	equ 9

	org 000H
	jmp begin

	; Start of our Operating System
GTU_OS:	PUSH D
	push D
	push H
	push psw
	nop	; This is where we run our OS in C++, see the CPU8080::isSystemCall()
		; function for the detail.
	pop psw
	pop h
	pop d
	pop D
	ret
	; ---------------------------------------------------------------
	; YOU SHOULD NOT CHANGE ANYTHING ABOVE THIS LINE        
	;Ahmet Alperen Bulut

;string:	dw 'Calistirilacak kod : ',00AH,00H ; null terminated string
comFile ds 25
begin:
;	LXI B, string
;	MVI A, PRINT_STR
;	call GTU_OS	
	LXI B, comFile
	MVI A,READ_STR
	call GTU_OS
	MVI A,FORK
	call GTU_OS
	MOV A,B
	CPI 0
	jz execs
	CPI 1
	JZ finish
	MOV B,A
	MVI A,WAIT_PID
	call GTU_OS
	jmp begin
execs: 
	LXI B,comFile
	MVI A,EXEC
	call GTU_OS

finish: 
	HLT
