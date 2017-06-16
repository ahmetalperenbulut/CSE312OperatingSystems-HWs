#ifndef H_GTUOS
#define H_GTUOS

#include "8080emuCPP.h"
#include <iostream>

class GTUOS{
public:

    uint64_t handleCall(const CPU8080 & cpu);
    void writeMemoryToFile(const CPU8080 & cpu,const std::string progName);
    void writeMemoryCout(const CPU8080 & cpu);
private:

    uint16_t print_b(const CPU8080 & cpu);
    uint16_t print_mem(const CPU8080 & cpu);
    uint16_t print_str(const CPU8080 & cpu);
    uint16_t read_b(const CPU8080 & cpu);
    uint16_t read_mem(const CPU8080 & cpu);
    uint16_t read_str(const CPU8080 & cpu);

};

#endif