//
// Created by Ahmet Alperen BULUT on 25.05.2017.
//

#ifndef HDD_H
#define HDD_H

#include <cstdlib>

#if defined __UINT32_MAX__ or UINT32_MAX
#include <inttypes.h>
#else
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;
#endif
class HDD {
public:
    HDD(){hardDisk=(uint8_t*) malloc(0x10000);}
    ~HDD(){free(hardDisk);}
    virtual uint8_t & at(uint32_t ind)throw() ;
    void setRegister(uint32_t baseRegister,uint32_t limitRegister) throw();
    virtual uint8_t& physicalAt(uint32_t ind) throw();
private:
    uint8_t * hardDisk;
    uint32_t baseRegister;
    uint32_t limitRegister;
};


#endif //HDD_H
