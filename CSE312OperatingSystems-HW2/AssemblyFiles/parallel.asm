        ; 8080 assembler code
        .hexfile parallel.hex
        .binfile parallel.com
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
prg1:	dw 'PrintNumbers.com',00H
prg2:	dw 'sum.com',00H
prg3:	dw 'PrintNumbersRev.com',00H
begin:
	MVI A,FORK
	call GTU_OS
	MOV A,B
	CPI 0
	jz exec1
	CPI 1
	JZ finish
	MVI A,FORK
	call GTU_OS
	MOV A,B
	CPI 0
	jz exec2
	CPI 1
	JZ finish
	MVI A,FORK
	call GTU_OS
	MOV A,B
	CPI 0
	jz exec3
	CPI 1
	JZ finish
finish:	
	HLT
exec1:
	LXI B,prg1
	MVI A,EXEC
	call GTU_OS
exec2:
	LXI B,prg2
	MVI A,EXEC
	call GTU_OS

exec3:
	LXI B,prg3
	MVI A,EXEC
	call GTU_OS
