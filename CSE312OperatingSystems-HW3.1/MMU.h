//
// Created by Ahmet Alperen BULUT on 20.04.2017.
//

#ifndef MMU_H
#define MMU_H

#include "memory.h"
class MMU : public memory {
public:
    MMU();
    virtual uint8_t & at(uint32_t ind) throw();
    virtual uint8_t & physicalAt(uint32_t ind) throw();
    void setRegister(uint32_t baseRegister,uint32_t limitRegister) throw();
private:
    uint32_t baseRegister;
    uint32_t limitRegister;
};


#endif //MMU_H
