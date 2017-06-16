//
// Created by Ahmet Alperen BULUT on 20.04.2017.
//

#ifndef PROCESS_H
#define PROCESS_H


#include <string>
#include "8080emuCPP.h"
enum State{ running , ready , blocked};
class process {
public:

    std::string getProcessName(){return processName;}
    const State8080 & getprocessRegister(){return processRegister;}
    uint16_t getBaseRegister(){return baseRegister;}
    uint16_t getLimitRegister(){return limitRegister;}
    uint16_t getPid(){return pid;}
    uint32_t getPpid(){return ppid;}
    uint64_t getProcessStartCycle(){return processStartCycle;}
    uint64_t getProcessUsingCycle(){return processUsingCycle;}
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
    void setPhysicalAddress(uint32_t pyhsicalAddress){this->physicalAddress=pyhsicalAddress;}
    void setState(State state){this->currentState=state;}
private:

    std::string processName;
    State8080 processRegister;
    uint16_t baseRegister;
    uint16_t limitRegister;
    uint16_t pid;
    uint16_t ppid;
    uint64_t processStartCycle;
    uint64_t processUsingCycle;
    uint32_t physicalAddress;
    State currentState;
};


#endif //PROCESS_H
