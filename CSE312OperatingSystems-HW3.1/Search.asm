        ; 8080 assembler code
        .hexfile Search.hex
        .binfile Search.com
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
	;Program girilen integer sayiyi asagida verilen arrayda arar bulursa indexini bulamazsa error yazar
integerList:	dw  0x12, 0x34, 0x53, 0x2A, 0x5B, 0x6F, 0x33, 0x21, 0x7C, 0xFF,  0xBA, 0xCB, 0xA1, 0x1A, 0x3B, 0xC3, 0x4A, 0x5D, 0x62, 0xA3, 0xB1, 0x5C, 0x7F,  0xCC, 0xAA, 0x34 ;26 eleman var
searchValue: ds 1

string:	dw 'Error',00AH,00H 

begin:

	LXI B, searchValue
	MVI A, READ_MEM	; store the OS call code to A
	call GTU_OS	; call the OS
	MVI B,0
	LXI H,integerList

for:

	MOV A,B
	CPI 26
	JZ notFound
	LDA searchValue
	CMP M
	JZ found
	INX H
	INX H
	INR B
	jmp for

found:
	
	MVI A,PRINT_B
	CALL GTU_OS
	JMP finish

notFound:

	LXI B,string
	MVI A,PRINT_STR
	CALL GTU_OS

finish:

	hlt