//
// Created by Ahmet Alperen BULUT on 25.05.2017.
//

#include "Process.h"
Process::Process(){
    for(short int i=0;i<16;++i){
        virtualTableAddress[i].virtualPageNumber=i;
        virtualTableAddress[i].pageFrameNumber=0;
        virtualTableAddress[i].present_Absent_Bit=false;
        virtualTableAddress[i].modifiedBit=false;
        virtualTableAddress[i].referencedBit=false;
    }
}
void Process::setBaseRegister(uint32_t baseRegister) {
    if(baseRegister>=0 && baseRegister<0x10000)
        this->baseRegister=baseRegister;
    else
        return;
}
void Process::setLimitRegister(uint32_t limitRegister) {
    if(limitRegister>=0 && limitRegister<=0x10000)
        this->limitRegister=limitRegister;
    else
        return;
}
void Process::setProcessRegister(State8080 processRegister) {
    this->processRegister=processRegister;
    /* this->processRegister.a=processRegister.a;
     this->processRegister.b=processRegister.b;
     this->processRegister.c=processRegister.c;
     this->processRegister.d=processRegister.d;
     this->processRegister.e=processRegister.e;
     this->processRegister.h=processRegister.h;
     this->processRegister.l=processRegister.l;
     this->processRegister.int_enable=processRegister.int_enable;
     this->processRegister.sp=processRegister.sp;
     this->processRegister.cc=processRegister.cc;
     this->processRegister.pc=processRegister.pc;*/
}
void Process::initialProcessRegister()  {
    this->processRegister.a=0;
    this->processRegister.b=0;
    this->processRegister.c=0;
    this->processRegister.d=0;
    this->processRegister.e=0;
    this->processRegister.h=0;
    this->processRegister.l=0;
    this->processRegister.int_enable=0;
    this->processRegister.sp=0x4000-1;
    this->processRegister.pc=0;
    this->processRegister.cc.ac=0;
    this->processRegister.cc.cy=0;
    this->processRegister.cc.pad=0;
    this->processRegister.cc.pad2=0;
    this->processRegister.cc.pad3=0;
    this->processRegister.cc.s=0;
    this->processRegister.cc.z=0;
}

void Process::setVirtualMemoryAddress(VirtualTableInformation *VirtualMemoryAddress) {
    for(short int i=0;i<16;++i){
        virtualTableAddress[i].referencedBit=VirtualMemoryAddress[i].referencedBit;
        virtualTableAddress[i].pageFrameNumber=VirtualMemoryAddress[i].pageFrameNumber;
        virtualTableAddress[i].modifiedBit=VirtualMemoryAddress[i].modifiedBit;
        virtualTableAddress[i].present_Absent_Bit=VirtualMemoryAddress[i].present_Absent_Bit;
    }
}


