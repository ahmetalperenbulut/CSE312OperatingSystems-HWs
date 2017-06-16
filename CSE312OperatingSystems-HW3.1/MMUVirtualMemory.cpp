//
// Created by Ahmet Alperen BULUT on 21.05.2017.
//

#include <iostream>
#include "MMUVirtualMemory.h"

MMUVirtualMemory::MMUVirtualMemory() {}

int MMUVirtualMemory::virtualAddressToPhysicalAddress(int virtualAddress)throw(PageFaultException){
    int physicalAddress = 0;
    int virtualPage = virtualAddress >> 10;
    int mask = 0;
    mask = mask | 0b1111111111;
    if(VirtualMemoryAddressTable[virtualPage].present_Absent_Bit) {
        VirtualMemoryAddressTable[virtualPage].referencedBit=true;
        physicalAddress = VirtualMemoryAddressTable[virtualPage].pageFrameNumber;
        physicalAddress = physicalAddress << 10;
        physicalAddress = physicalAddress|(virtualAddress & mask);
        return physicalAddress;
    }
    else {

        throw PageFaultException("alp",virtualPage);//pagefault exception yaz
    }
    return 0;
}
void MMUVirtualMemory::setModifiedBit(int virtualAddress)throw(PageFaultException){

    int virtualPage = virtualAddress >> 10;

    if(VirtualMemoryAddressTable[virtualPage].present_Absent_Bit) {
        VirtualMemoryAddressTable[virtualPage].referencedBit=true;
        VirtualMemoryAddressTable[virtualPage].modifiedBit=true;

    }
    else {

        throw PageFaultException("alp",virtualPage);//pagefault exception yaz
    }
    return ;
}
uint8_t& MMUVirtualMemory::at(uint32_t ind)throw(PageFaultException) {
    int physicalAddress=virtualAddressToPhysicalAddress(ind);
    return memory::at(physicalAddress);
}
uint8_t& MMUVirtualMemory::physicalAt(uint32_t ind)throw(PageFaultException) {
    return memory::physicalAt(ind);
}
void MMUVirtualMemory::setVirtualMemoryAddressTable(VirtualTableInformation* virtualMemoryAddress) {
    VirtualMemoryAddressTable=virtualMemoryAddress;
}
bool MMUVirtualMemory::isTherePage(uint32_t ind) {
    try{
        virtualAddressToPhysicalAddress(ind);
        return true;
    }
    catch (PageFaultException &exp){
        return false;
    }
}

void MMUVirtualMemory::setProcessRegister(State8080 processRegister) {
    this->recoveryState=processRegister;
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