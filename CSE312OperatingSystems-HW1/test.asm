        ; 8080 assembler code
        .hexfile test.hex
        .binfile test.com
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

	; Position for stack pointer
stack   equ 0F000h

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
value: db 0x5
string: dw 'Merhaba Hocam, Kolay Gelsin',00AH,00H 
yaz: ds 51
begin:
	MVI B,5
	MVI A,PRINT_B
	CALL GTU_OS
	LXI B,value
	MVI A,PRINT_MEM
	CALL GTU_OS
	LXI B,string
	MVI A,PRINT_STR
	call GTU_OS
	MVI A,READ_B
	call GTU_OS
	LXI B,value
	MVI A,READ_MEM
	CALL GTU_OS
	LXI B,yaz
	MVI A,READ_STR
	call GTU_OS
	LXI B,yaz
	MVI A,PRINT_STR
	call GTU_OS

	hlt



