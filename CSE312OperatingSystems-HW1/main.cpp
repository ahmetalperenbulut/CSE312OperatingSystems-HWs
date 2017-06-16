#include <iostream>
#include "8080emuCPP.h"
#include "gtuos.h"

int main (int argc, char**argv)

{
    if (argc != 3){
        std::cerr << "Usage: prog exeFile debugOption\n";
        exit(1);
    }

    int DEBUG = atoi(argv[2]);
    uint16_t cycles=0;
    CPU8080 theCPU;
    GTUOS	theOS;

    theCPU.ReadFileIntoMemoryAt(argv[1], 0x0000);

    do
    {
        cycles+=theCPU.Emulate8080p(DEBUG);
        if(theCPU.isSystemCall())
            cycles+=(int16_t)theOS.handleCall(theCPU);

        if(DEBUG==2)
            std::cin.get();
        // theOS.writeMemoryCout(theCPU);
    }	while (!theCPU.isHalted());

    theOS.writeMemoryToFile(theCPU,argv[1]);
    std::cout<<std::dec<<"Total Number Of Cycles: "<<cycles<<std::endl;

    return 0;
}