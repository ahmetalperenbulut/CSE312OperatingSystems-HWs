//
// Created by Ahmet Alperen BULUT on 17.04.2017.
//

#ifndef H_GTUOS
#define H_GTUOS

#include "8080emuCPP.h"
#include <iostream>
#include <queue>
#include <vector>
#include "process.h"
#include "MMU.h"
#include <stdio.h>
class GTUOS{
public:
    GTUOS(CPU8080& cpu,MMU& memory,uint16_t DEBUG);
    uint64_t handleCall();
    void createProcess(std::string processName,uint16_t ppid=0);
    void saveProcess();
    void changeProcess();
    void terminateProcess();
    void writeMemoryToFile(const std::string progName);
    uint32_t incrementCycles(uint32_t cycle){this->cycles+=cycle; return cycle;}
    uint32_t getCycles(){return cycles;}
    void writeMemoryCout();
   // void terminateProcess(uint16_t pid);
    bool isFinish();
    bool isReadyQueueEmpty(){return readyQueue.empty();}
    uint16_t getRunningProcessCount(){return runningProcessCount;}
private:

    uint16_t print_b(const CPU8080 & cpu);
    uint16_t print_mem(const CPU8080 & cpu);
    uint16_t print_str(const CPU8080 & cpu);
    uint16_t read_b(const CPU8080 & cpu);
    uint16_t read_mem(const CPU8080 & cpu);
    uint16_t read_str(const CPU8080 & cpu);
    uint16_t fork();
    uint16_t exec();
    uint16_t waitpid(const CPU8080 & cpu);
    void saveProcess(const CPU8080 & cpu,State state,uint16_t waitPid);
    void createProcessForProcess(std::string processName,uint16_t ppid);
    void changeCPUStateRegister();
    void writeInformation();
    CPU8080 & cpu;
    MMU & memory;
    process* processTable[4];
    std::queue < std::pair<int,int> > readyQueue;
    std::vector < std::pair<int,int> > blockedQueue;
    short int runningProcessCount;
    short int runningProcess;
    uint16_t allProcessCount;
    uint32_t cycles;
    uint32_t tempCycles;
    uint16_t debugMode;
};

#endif


