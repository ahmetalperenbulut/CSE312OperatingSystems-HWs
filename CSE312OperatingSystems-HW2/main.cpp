

#include <iostream>

#include "gtuos.h"

int main (int argc, char**argv)

{
    if (argc != 3){
        std::cerr << "Usage: prog exeFile debugOption\n";
        exit(1);
    }

    uint16_t DEBUG = atoi(argv[2]);

    MMU mem;
    CPU8080 theCPU(&mem);
    GTUOS	theOS(theCPU, mem,DEBUG);
    const uint16_t quantum=100;
    uint32_t cycles=0;

    theOS.createProcess(argv[1],0);
    theOS.changeProcess();

    do
    {
        cycles+=theOS.incrementCycles(theCPU.Emulate8080p(DEBUG));
        if(theCPU.isSystemCall())
            cycles += theOS.incrementCycles(theOS.handleCall());

        if(theCPU.isHalted()){
            theOS.terminateProcess();
            theOS.changeProcess();
            cycles=0;
        }
        else if(cycles>=quantum && !theOS.isReadyQueueEmpty()){
            theOS.saveProcess();
            theOS.changeProcess();
            cycles=0;
        }
        // theOS.writeMemoryCout(theCPU);
    }	while (!theOS.isFinish());

    theOS.writeMemoryToFile(argv[1]);
    std::cout<<std::dec<<"Total Number Of Cycles: "<<theOS.getCycles()<<std::endl;

    return 0;
}