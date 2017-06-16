//
// Created by Ahmet Alperen BULUT on 25.05.2017.
//

#ifndef PROCESS_H
#define PROCESS_H


#include <string>
#include <vector>
#include <queue>
#include "8080emuCPP.h"
#include "MMUVirtualMemory.h"

enum State{ running , ready , blocked};

class Process {
public:
    Process();
    std::string getProcessName(){return processName;}
    const State8080 & getprocessRegister(){return processRegister;}
    uint16_t getBaseRegister(){return baseRegister;}
    uint16_t getLimitRegister(){return limitRegister;}
    uint16_t getPid(){return pid;}
    uint32_t getPpid(){return ppid;}
    long long int getProcessStartCycle(){return processStartCycle;}
    long long int getProcessUsingCycle(){return processUsingCycle;}
    uint64_t getPhysicalAddress(){return physicalAddress;}
    State getState(){ return currentState;}
    void initialProcessRegister();
    void setProcessName(std::string processName){this->processName=processName;}
    void setProcessRegister(State8080 processRegister);
    void setBaseRegister(uint32_t baseRegister);
    void setLimitRegister(uint32_t limitRegister);
    void setPid( uint16_t pid){this->pid=pid;}
    void setPpid(uint16_t ppid){this->ppid=ppid;}
    void setProcessStartCycle(uint64_t processStartCycle){this->processStartCycle=processStartCycle;}
    void setProcessUsingCycle(uint64_t processUsingCycle){this->processUsingCycle=processUsingCycle;}
    //void setPhysicalAddress(uint32_t pyhsicalAddress){this->physicalAddress=pyhsicalAddress;}
    void setState(State state){this->currentState=state;}
    void setVirtualMemoryAddress(VirtualTableInformation * VirtualMemoryAddress);
    VirtualTableInformation * getVirtalMemoryAddress(){return virtualTableAddress;}
    std::queue<VirtualTableInformation*>& getFifoVT(){return fifoVT;}
private:

    std::string processName;
    State8080 processRegister;
    uint16_t baseRegister;
    uint16_t limitRegister;
    uint16_t pid;
    uint16_t ppid;
    long long int processStartCycle;
    long long int processUsingCycle;
    uint32_t physicalAddress;
    State currentState;
    VirtualTableInformation virtualTableAddress[16];
    std::queue<VirtualTableInformation*> fifoVT;
};


#endif //INC_151044080_HW3_PROCESS_H
