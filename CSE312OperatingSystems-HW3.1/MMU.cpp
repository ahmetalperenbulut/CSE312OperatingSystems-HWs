//
// Created by Ahmet Alperen BULUT on 20.04.2017.
//

#include "MMU.h"
#include <iostream>
#include <stdexcept>

MMU::MMU() {
    this->baseRegister=0;
    this->limitRegister=0;
}
uint8_t& MMU::at(uint32_t ind) throw(){
    if(ind>=0x4000 || ind < 0){
        throw new std::range_error("Segmentation Fault");
    }
    if(baseRegister+ind<=limitRegister)
        return memory::physicalAt(baseRegister+ind);
    else
        throw new std::range_error("Segmentation Fault");
    //fizikselde direk verilen atte ekle
}

uint8_t& MMU::physicalAt(uint32_t ind) throw(){
    if(ind>0x10000 || ind < 0){
        throw new std::range_error("Segmentation Fault");
    }
    return memory::physicalAt(ind);
}

void MMU::setRegister(uint32_t baseRegister, uint32_t limitRegister) throw(){
    if(baseRegister<0 || limitRegister>0x10000  || limitRegister-baseRegister>0x4000)
        throw new std::range_error("Hatali register değerleri");
    this->baseRegister=baseRegister;
    this->limitRegister=limitRegister;

}
