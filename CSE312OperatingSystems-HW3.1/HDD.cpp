//
// Created by Ahmet Alperen BULUT on 25.05.2017.
//

#include <stdexcept>
#include <iostream>
#include "HDD.h"

uint8_t& HDD::at(uint32_t ind) throw(){
    if(ind>=0x4000 || ind < 0){
        std::cerr<<"ben"<<ind;
        throw new std::range_error("Segmentation Fault");
    }
    if(baseRegister+ind<=limitRegister)
        return hardDisk[baseRegister+ind];
    else{std::cerr<<"ben2";
        throw new std::range_error("Segmentation Fault");}
}

uint8_t& HDD::physicalAt(uint32_t ind) throw(){
    if(ind>0x10000 || ind < 0){
        std::cerr<<"ben3";
        throw new std::range_error("Segmentation Fault");
    }
    return hardDisk[ind];
}

void HDD::setRegister(uint32_t baseRegister, uint32_t limitRegister) throw(){
    if(baseRegister<0 || limitRegister>0x10000  || limitRegister-baseRegister>0x4000){
        std::cerr<<"ben4";
        throw new std::range_error("Hatali register değerleri");}
    this->baseRegister=baseRegister;
    this->limitRegister=limitRegister;

}