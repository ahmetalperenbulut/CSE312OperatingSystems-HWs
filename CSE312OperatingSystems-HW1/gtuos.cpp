
#include "gtuos.h"
#include <fstream>
#include <iostream>
#include <iomanip>

uint64_t GTUOS::handleCall(const CPU8080 & cpu){
    uint64_t cycles=0;
    switch (cpu.state->a){
        case 1:
            cycles=print_b(cpu);
            break;
        case 2:
            cycles=print_mem(cpu);
            break;
        case 3:
            cycles=read_b(cpu);
            break;
        case 4:
            cycles=read_mem(cpu);
            break;
        case 5:
            cycles=print_str(cpu);
            break;
        case 6:
            cycles=read_str(cpu);
            break;
        default:
            std::cout <<  "Unimplemented OS call";
            throw -1;
            break;
    }
    return cycles;
}


uint16_t GTUOS::print_b(const CPU8080 & cpu){
    std::cout<<"System Call:PRINT_B\n"<<"Value Of B: "<<std::endl;
    std::cout<<std::dec<<"Dec: "<< (int)cpu.state->b << std::endl;
    std::cout<<std::hex<<"Hex: 0x" <<(int)cpu.state->b<<std::endl;
    return 10;
}

uint16_t GTUOS::print_mem(const CPU8080 &cpu) {
    uint16_t address = cpu.state->b;
    address = (address<<8) | cpu.state->c;
    std::cout<<"System Call:PRINT_MEM\n"<<"Value Of MEM: "<<std::endl;
    std::cout<<std::dec<<"Dec: "<< (int)cpu.memory[address] << std::endl;
    std::cout<<std::hex<<"Hex: 0x" <<(int)cpu.memory[address] << std::endl;
    return 10;
}

uint16_t GTUOS::read_b(const CPU8080 & cpu){
    uint16_t value,mask = 0b0000000011111111;
    std::cout<<"System Call:READ_B\n"<<"Value: ";
    std::cin >> value;
    value = value & mask;
    cpu.state->b = value;
    return 10;
}

uint16_t GTUOS::read_mem(const CPU8080 & cpu){
    uint16_t value,mask = 0b0000000011111111;
    uint16_t address = cpu.state->b;
    address = (address<<8) | cpu.state->c;
    std::cout<<"System Call:READ_MEM\n"<<"Value: ";
    std::cin>>value;
    value = value & mask;
    cpu.memory[address] = value;
    return 10;
}

uint16_t GTUOS::print_str(const CPU8080 & cpu){
    uint16_t address = cpu.state->b;
    address = (address<<8) | cpu.state->c;
    std::cout<<"System Call:PRINT_STR\n"<<"Value Of Str: ";
    while( cpu.memory[address] != '\0' ){
        std::cout << cpu.memory[address] ;
        ++address;
    }
    std::cout<<std::endl;
    return 100;
}

uint16_t GTUOS::read_str(const CPU8080 & cpu){
    uint16_t address = cpu.state->b;
    address = (address<<8) | cpu.state->c;
    char value[51];

    std::cout<<"System Call:READ_STR\n"<<"Value Of: ";
    std::cin.ignore();
    std::cin.getline(value,50);

    for(uint16_t i=0;i<strlen(value);++i )
    {
        cpu.memory[address]=value[i];
        ++address;
    }
    /* for(char p : value){
         cpu.memory[address] = p;
         ++address;
     }*/
    return 100;
}

void GTUOS::writeMemoryToFile(const CPU8080 & cpu,const std::string progName){
    std::ofstream myfile;
    int temp=0;
    std::string memName=progName.substr(0,progName.length()-4);
    memName.insert(memName.length(),".mem");
    myfile.open (memName.c_str(), std::ios_base::binary | std::ios_base::out);
    myfile  <<std::setiosflags(std::ios_base::left)<<std::setw(7)<<std::setfill(' ')<<std::dec <<"1 ";
    for(int i=0;i<0x10000;++i) {
        ++temp;
        myfile <<"0x";
        myfile <<std::hex<<std::setfill('0')<<std::setw(2) <<(int16_t)cpu.memory[i]<<" ";
        if(temp==16 ){
            myfile << std::endl <<std::setw(6)<<std::setfill(' ')<<std::dec<< i+1 <<" ";
            temp=0;
        }
    }
    myfile.close();
}
void GTUOS::writeMemoryCout(const CPU8080 & cpu) {
    for (int i=13;i<=75;++i){
        std::cout<<std::hex<<(int16_t)cpu.memory[i]<<" ";
        if(i==64)
            std::cout<<" | ";}
    std::cout<<std::endl;

}