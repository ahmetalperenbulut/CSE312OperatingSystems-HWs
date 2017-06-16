
#include "gtuos.h"
#include <fstream>
#include <iostream>
#include <iomanip>

GTUOS::GTUOS(CPU8080 &otherCpu,MMU &otherMemory,uint16_t DEBUG):cpu(otherCpu),memory(otherMemory) {
    runningProcessCount=0;
    runningProcess=-1;
    allProcessCount=1;
    cycles=0;
    tempCycles=cycles;
    debugMode=DEBUG;
    for(int i=0;i<4;++i)
        processTable[i]= NULL;
}

uint64_t GTUOS::handleCall(){
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
        case 7:
            cycles=fork();
            break;
        case 8:
            cycles=exec();
            break;
        case 9:
            cycles=waitpid(cpu);
            break;
        default:
            std::cout <<  "Unimplemented OS call";
            throw -1;
            break;
    }
    return cycles;
}

uint16_t GTUOS::print_b(const CPU8080 & cpu){
    std::cout<<"System Call:PRINT_B : "<<std::endl;
    std::cout<<std::dec<<"Dec: "<< (int)cpu.state->b << std::endl;
    std::cout<<std::hex<<"Hex: 0x" <<(int)cpu.state->b<<std::endl;
    return 10;
}

uint16_t GTUOS::print_mem(const CPU8080 &cpu) {
    uint16_t address = cpu.state->b;
    address = (address<<8) | cpu.state->c;
    std::cout<<"System Call:PRINT_MEM : "<<std::endl;
    std::cout<<std::dec<<"Dec: "<< (int)cpu.memory->at(address) << std::endl;
    std::cout<<std::hex<<"Hex: 0x" <<(int)cpu.memory->at(address) << std::endl;
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
    memory.at(address) = value;
    return 10;
}

uint16_t GTUOS::print_str(const CPU8080 & cpu){
    uint16_t address = cpu.state->b;
    address = (address<<8) | cpu.state->c;
    std::cout<<"System Call:PRINT_STR\n";
    while( memory.at(address) != '\0' ){
        std::cout << memory.at(address) ;
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
    //std::cin.ignore();
    std::cin.getline(value,50);

    for(uint16_t i=0;i<=strlen(value);++i )
    {
        cpu.memory->at(address)=value[i];
        ++address;
    }
    /* for(char p : value){
         cpu.memory[address] = p;
         ++address;
     }*/
    return 100;
}

uint16_t GTUOS::fork(){
    if(runningProcessCount>=4)
    {
        cpu.state->b=1;
    } else{
        cpu.state->b=0;//cocuga 0 gitsin diye
        createProcessForProcess(processTable[runningProcess]->getProcessName()+" Child",processTable[runningProcess]->getPid());
        cpu.state->b=allProcessCount;
    }
    return 50;
}

uint16_t GTUOS::exec(){
    uint16_t address = cpu.state->b;
    address = (address<<8) | cpu.state->c;
    std::string fileName;
    while( memory.at(address) != '\0' ){
        fileName.append(1,memory.at(address)) ;
        ++address;
    }
    cpu.ReadFileIntoMemoryAt(fileName.c_str(),processTable[runningProcess]->getBaseRegister());
    processTable[runningProcess]->setProcessName(fileName.c_str());
    processTable[runningProcess]->initialProcessRegister();
    *(cpu.state)=processTable[runningProcess]->getprocessRegister();
    return 80;
}

uint16_t GTUOS::waitpid(const CPU8080 & cpu){
    saveProcess(cpu,blocked,(uint16_t)cpu.state->b);
    changeProcess();
    return 80;
}

void GTUOS::createProcessForProcess(std::string processName,uint16_t ppid) {
    if (runningProcessCount >= 4)
        return;
    else {
        int i;
        for(i=0;i<4;++i)
        {
            if(processTable[i]== NULL) {
                processTable[i] = new process();
                break;
            }
        }
        processTable[i]->setProcessName(processName);
        processTable[i]->setBaseRegister(i*0x4000);
        processTable[i]->setLimitRegister(((i+1)*0x4000)-1);
        processTable[i]->setPid(allProcessCount+1);
        processTable[i]->setPpid(ppid);
        processTable[i]->setProcessStartCycle(cycles);
        processTable[i]->setProcessUsingCycle(0);
        processTable[i]->setProcessRegister(*cpu.state);
        processTable[i]->setState(ready);

        uint8_t *buffer =  &memory.physicalAt(processTable[i]->getBaseRegister());
        uint8_t *parentBuffer=&memory.physicalAt(processTable[runningProcess]->getBaseRegister());
        memcpy(buffer,parentBuffer,0x4000);
        readyQueue.push(std::make_pair(i,processTable[i]->getPid()));

        ++runningProcessCount;
        ++allProcessCount;
    }
}

void GTUOS::createProcess(std::string processName,uint16_t ppid) {
    if (runningProcessCount >= 4)
        return;
    else {
        int i;
        for(i=0;i<4;++i)
        {
            if(processTable[i]== NULL) {
                processTable[i] = new process();
                break;
            }
        }
        processTable[i]->setProcessName(processName);
        processTable[i]->setBaseRegister(i*0x4000);
        processTable[i]->setLimitRegister(((i+1)*0x4000)-1);
        processTable[i]->setPid(allProcessCount+1);
        processTable[i]->setPpid(ppid);
        processTable[i]->setProcessStartCycle(cycles);
        processTable[i]->setProcessUsingCycle(0);
        processTable[i]->initialProcessRegister();
        processTable[i]->setState(ready);

        cpu.ReadFileIntoMemoryAt(processName.c_str(), processTable[i]->getBaseRegister());
        readyQueue.push(std::make_pair(i,processTable[i]->getPid()));

        ++runningProcessCount;
        ++allProcessCount;
    }
}
void GTUOS::changeProcess() {
    if(runningProcessCount<=0)
        return;
    if(!readyQueue.empty()){
        processTable[(readyQueue.front()).first]->setState(running);
        if(debugMode>=2)
            writeInformation();
        runningProcess=(readyQueue.front()).first;
        readyQueue.pop();
        changeCPUStateRegister();
        memory.setRegister(processTable[runningProcess]->getBaseRegister(),processTable[runningProcess]->getLimitRegister());
    }
}
void GTUOS::changeCPUStateRegister(){
    tempCycles=cycles;
    //register
    *(cpu.state)=processTable[runningProcess]->getprocessRegister();
/*
    cpu.state->a=processTable[runningProcess]->getprocessRegister().a;
    cpu.state->b=processTable[runningProcess]->getprocessRegister().b;
    cpu.state->c=processTable[runningProcess]->getprocessRegister().c;
    cpu.state->d=processTable[runningProcess]->getprocessRegister().d;
    cpu.state->e=processTable[runningProcess]->getprocessRegister().e;
    cpu.state->h=processTable[runningProcess]->getprocessRegister().h;
    cpu.state->l=processTable[runningProcess]->getprocessRegister().l;
    //other

    cpu.state->int_enable=processTable[runningProcess]->getprocessRegister().int_enable;
    cpu.state->pc=processTable[runningProcess]->getprocessRegister().pc;
    cpu.state->sp=processTable[runningProcess]->getprocessRegister().sp;
    cpu.state->cc.s=processTable[runningProcess]->getprocessRegister().cc.s;
    cpu.state->cc.pad3=processTable[runningProcess]->getprocessRegister().cc.pad3;
    cpu.state->cc.pad=processTable[runningProcess]->getprocessRegister().cc.pad;
    cpu.state->cc.pad2=processTable[runningProcess]->getprocessRegister().cc.pad2;
    cpu.state->cc.p=processTable[runningProcess]->getprocessRegister().cc.p;
    cpu.state->cc.ac=processTable[runningProcess]->getprocessRegister().cc.ac;
    cpu.state->cc.cy=processTable[runningProcess]->getprocessRegister().cc.cy;
    cpu.state->cc.z=processTable[runningProcess]->getprocessRegister().cc.z;
*/
}

void GTUOS::saveProcess(const CPU8080 & cpu,State state,uint16_t waitPid) {
    processTable[runningProcess]->setProcessRegister(*cpu.state);
    processTable[runningProcess]->setState(state);
    processTable[runningProcess]->setProcessUsingCycle(processTable[runningProcess]->getProcessUsingCycle()+cycles-tempCycles);
    if(state==ready)
        readyQueue.push(std::make_pair(runningProcess,processTable[runningProcess]->getPid()));
    else
        blockedQueue.push_back(std::make_pair(runningProcess,waitPid));
}

void GTUOS::saveProcess() {
    processTable[runningProcess]->setProcessRegister(*cpu.state);
    processTable[runningProcess]->setState(ready);
    processTable[runningProcess]->setProcessUsingCycle(processTable[runningProcess]->getProcessUsingCycle()+cycles-tempCycles);
    readyQueue.push(std::make_pair(runningProcess,processTable[runningProcess]->getPid()));
}

void GTUOS::terminateProcess(){
    int i=blockedQueue.size()-1;
    processTable[runningProcess]->setProcessRegister(*cpu.state);
    processTable[runningProcess]->setProcessUsingCycle(processTable[runningProcess]->getProcessUsingCycle()+(cycles-tempCycles));
    writeMemoryToFile(processTable[runningProcess]->getProcessName());
    std::cout<<"Bir Process Sonlaniyor\nProcess Pid : "<<processTable[runningProcess]->getPid()<<" "<<std::endl;
    std::cout<<"Process Name : "<<processTable[runningProcess]->getProcessName()<<std::endl;
    std::cout<<std::dec<<"Using Cycles : "<<processTable[runningProcess]->getProcessUsingCycle()<<std::endl;

    while(i>=0){
        if(blockedQueue[i].second==processTable[runningProcess]->getPid()) {
            readyQueue.push(std::make_pair(blockedQueue[i].first, processTable[blockedQueue[i].first]->getPid()));
            blockedQueue.erase(blockedQueue.begin() + i);
        }
        --i;
    }
    delete(processTable[runningProcess]);
    processTable[runningProcess]= NULL;
    --runningProcessCount;

}

bool GTUOS::isFinish() {
    return runningProcessCount == 0 ? true : false;
}

void GTUOS::writeInformation() {
    if(debugMode==2) {
        if (runningProcessCount == 1 && !isFinish()) {
            for (int i = 0; i < 4; ++i) {
                if (processTable[i] != NULL)
                    std::cout << "Running Process Name : " << processTable[i]->getProcessName() << std::endl;
            }
        } else if (runningProcessCount > 1) {
            if (processTable[runningProcess] != NULL) {
                std::cout << std::dec << "Blocked Process Name : " << processTable[runningProcess]->getProcessName()
                          << " using cycles : " << processTable[runningProcess]->getProcessUsingCycle() << std::endl;
                std::cout << "Running Process Name : " << processTable[readyQueue.front().first]->getProcessName()
                          << std::endl;

            } else
                std::cout << "Running Process Name : " << processTable[readyQueue.front().first]->getProcessName()
                          << std::endl;
        }
    }
    if(debugMode==3){
        std::printf("\n%-20s Pid \t PPid \t %-9s %-14s %-13s %-8s %-8s\n","Process Name","State","Start Cycle","Using Cycle","Base","Limit");
        std::string state;
        for(int i=0;i<4;++i){
            if(processTable[i]!=NULL){
                if(processTable[i]->getState()==ready)
                    state="ready";
                else if(processTable[i]->getState()==blocked)
                    state="blocked";
                else
                    state="runnig";

                std::printf("%-20s %-5d \t %-5d \t %-9s %-14lld %-13lld %-8d %-8d \n",processTable[i]->getProcessName().c_str(),
                            processTable[i]->getPid(),processTable[i]->getPpid(),state.c_str()
                        ,processTable[i]->getProcessStartCycle(),processTable[i]->getProcessUsingCycle()
                        ,processTable[i]->getBaseRegister(),processTable[i]->getLimitRegister());
            }
        }
        std::cout<<std::endl;
    }
}

void GTUOS::writeMemoryToFile(const std::string progName){
    std::ofstream myfile;
    int temp=0;
    std::string memName=progName.substr(0,progName.length()-4);
    memName.insert(memName.length(),".mem");
    myfile.open (memName.c_str(), std::ios_base::binary | std::ios_base::out);
    myfile  <<std::setiosflags(std::ios_base::left)<<std::setw(7)<<std::setfill(' ')<<std::dec <<"1 ";
    for(int i=0;i<0x4000;++i) {
        ++temp;
        myfile <<"0x";
        myfile <<std::hex<<std::setfill('0')<<std::setw(2) <<(int16_t)memory.at(i)<<" ";
        if(temp==16 ){
            myfile << std::endl <<std::setw(6)<<std::setfill(' ')<<std::dec<< i+1 <<" ";
            temp=0;
        }
    }
    myfile.close();
}

void GTUOS::writeMemoryCout() {
    for (int i=13;i<=75;++i){
        std::cout<<std::hex<<(int16_t)cpu.memory->at(i)<<" ";
        if(i==64)
            std::cout<<" | ";}
    std::cout<<std::endl;
}
