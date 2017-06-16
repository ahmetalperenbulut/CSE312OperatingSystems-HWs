//
// Created by Ahmet Alperen BULUT on 21.05.2017.
//

#ifndef MMUVIRTUALMEMORY_H
#define MMUVIRTUALMEMORY_H

#include "memory.h"
#include "PageFaultException.h"
#include "8080emuCPP.h"

struct VirtualTableInformation{
    bool		modifiedBit;
    bool		referencedBit;
    bool		present_Absent_Bit;
    int         pageFrameNumber;
    int         virtualPageNumber;
};
class MMUVirtualMemory: public memory{
    friend class GTUOS;
    friend class CPU8080;
public:
    MMUVirtualMemory();
    uint8_t & at(uint32_t ind)throw(PageFaultException);
    uint8_t & physicalAt(uint32_t ind)throw(PageFaultException);
    void setVirtualMemoryAddressTable(VirtualTableInformation* virtualMemoryAddress);
    VirtualTableInformation* getVirtualMemoryAddressTable(){return VirtualMemoryAddressTable;}

    void setProcessRegister(State8080 processRegister);
    void setModifiedBit(int virtualAddress)throw(PageFaultException);
private:
    int virtualAddressToPhysicalAddress(int virtualAddress)throw(PageFaultException);//10bitlik offset 1 kb oldugu icin pageler

    bool isTherePage(uint32_t ind);
    VirtualTableInformation* VirtualMemoryAddressTable;
    State8080 recoveryState;
    //4 bit pageleri gosterecek
    //virtual address bundan dolay 2^14 olmalı giris
    //physical address ise 2^13 olmalı 8 kb oldugundan

};
#endif //MMUVIRTUALMEMORY_H
