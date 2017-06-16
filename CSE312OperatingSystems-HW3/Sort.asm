   ; 8080 assembler code
        .hexfile Sort.hex
        .binfile Sort.com
        ; try "hex" for downloading in hex format
        .download bin  
        .objcopy gobjcopy
        .postbuild echo "OK!"
        ;.nodump

    ; OS call list
PRINT_B        equ 1
PRINT_MEM    equ 2
READ_B        equ 3
READ_MEM    equ 4
PRINT_STR    equ 5
READ_STR    equ 6

    org 000H
    jmp begin

    ; Start of our Operating System
GTU_OS:    PUSH D
    push D
    push H
    push psw
    nop    ; This is where we run our OS in C++, see the CPU8080::isSystemCall()
        ; function for the detail.
    pop psw
    pop h
    pop d
    pop D
    ret
    ; ---------------------------------------------------------------
    ; YOU SHOULD NOT CHANGE ANYTHING ABOVE THIS LINE
    ;Ahmet Alperen Bulut   
    integerList:    dw  0x12, 0x34, 0x53, 0x2A, 0x5B, 0x6F, 0x33, 0x21, 0x7C, 0xFF,  0xBA, 0xCB, 0xA1, 0x1A, 0x3B, 0xC3, 0x4A, 0x5D, 0x62, 0xA3, 0xB1, 0x5C, 0x7F,  0xCC, 0xAA, 0x34 ;26 eleman var
    flag: db 1
    tempLength: db 25

begin:
    LXI H,integerList
    MVI B, 0 ;for1 icin
    
for1:

	INR B
    MOV A,B
    CPI 25
    JZ print
    MVI A,1
    STA flag
    MVI A,26
    SUB B
    STA tempLength
    MVI C, 0 ; for2 icin
    LXI H,integerList

for2:        

   LDA tempLength
    MOV D,A
    MOV A,C
    CPI 25
    jz atla
 
    MOV A,M
    ;MOV D,H
    ;MOV E,L efficient olmadıgı icin alt tarafa tasındı 
    INX H
    INX H
    CMP M
    JC noSwap ; j > j+1
    push B
    MOV D,H
    MOV E,L
    DCX D
    DCX D
    MOV B,M
    MOV M,A
    MOV a,B
    STAx D
    pop B
    MVI A,0
    STA flag

noSwap:
    INR C
    jmp for2

 atla:

 	LDA flag
	CPI 1
	JZ print
	jmp for1

print:	
	MVI C,0
	LXI H,integerList

for3:

	MOV A,C
	CPI 26
	jz finish
	MOV B,M
	MVI A, PRINT_B
	call GTU_OS
	INX H
	INX H
	INR C
	jmp for3

finish:	
	HLT