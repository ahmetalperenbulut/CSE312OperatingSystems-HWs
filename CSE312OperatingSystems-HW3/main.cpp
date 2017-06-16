#include <iostream>
#include <cstdlib>
#include "MMUVirtualMemory.h"
#include "8080emuCPP.h"
#include "gtuos.h"


int main (int argc, char**argv)
{
    if (argc != 3){
        std::cerr << "Usage: prog exeFile debugOption\n";
        exit(1);
    }

    uint16_t DEBUG = atoi(argv[2]);
    MMUVirtualMemory mem;
    CPU8080 theCPU(&mem);
    HDD swap;
    GTUOS	theOS(theCPU, mem,swap,DEBUG);
    const uint16_t quantum=100;
    uint32_t cycles=0;
    theOS.createProcess(argv[1],0);
    theOS.changeProcess();

    do
    {
        try {
            cycles+= theOS.incrementCycles(theCPU.Emulate8080p(DEBUG));

            if (theCPU.isSystemCall())
                cycles += theOS.incrementCycles(theOS.handleCall());

            if (theCPU.isHalted()) {
                theOS.terminateProcess();
                theOS.changeProcess();
                cycles = 0;
            }
            else if (cycles >= quantum && !theOS.isReadyQueueEmpty()) {
                theOS.saveProcess();
                theOS.changeProcess();
                cycles = 0;
            }
            if(DEBUG==1 || DEBUG==2)
                theOS.writeInformation();
            if(DEBUG==2)
                std::cin.get();
        }
        catch (PageFaultException &exp){
            theOS.pageFault(exp.getPageFrame());
        }
    }	while (!theOS.isFinish());

//        theOS.writeMemoryToFile(argv[1]);
    std::cout<<std::dec<<"Total Number Of Cycles: "<<theOS.getCycles()<<std::endl;
    return 0;
}