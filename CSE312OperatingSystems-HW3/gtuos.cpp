//
// Created by Ahmet Alperen BULUT on 21.05.2017.
//
#include "gtuos.h"
#include <fstream>
#include <iomanip>

GTUOS::GTUOS(CPU8080 &otherCpu,MMUVirtualMemory &otherMemory,HDD& hardDisk,uint16_t DEBUG):
        cpu(otherCpu),memory(otherMemory),hardDisk(hardDisk) {
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

    if(!memory.isTherePage(address)){
        int virtualPage = address >> 10;
        loadPage(virtualPage);
    }

    memory.setModifiedBit(address);
    memory.at(address) = value;

    return 10;
}

uint16_t GTUOS::print_str(const CPU8080 & cpu){
    uint16_t address = cpu.state->b;
    address = (address<<8) | cpu.state->c;
    std::cout<<"System Call:PRINT_STR\n";
    if(!memory.isTherePage(address)){
        int virtualPage = address >> 10;
        loadPage(virtualPage);
    }
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
    if(!memory.isTherePage(address)){
        int virtualPage = address >> 10;
        loadPage(virtualPage);
    }
    for(uint16_t i=0;i<=strlen(value);++i ) {
        memory.setModifiedBit(address);
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
    saveMemoryToSwap();
    if(runningProcessCount>=4) {
        cpu.state->b=1;
    } else{
        cpu.state->b=0;//cocuga 0 gitsin diye
        createProcessForProcess(processTable[runningProcess]->getProcessName()+" Child",processTable[runningProcess]->getPid());
        cpu.state->b=allProcessCount;
    }

    return 50;
}

uint16_t GTUOS::exec(){
    saveMemoryToSwap();
    uint16_t address = cpu.state->b;
    address = (address<<8) | cpu.state->c;
    std::string fileName;

    if(!memory.isTherePage(address)){
        int virtualPage = address >> 10;
        loadPage(virtualPage);
    }

    while( memory.at(address) != '\0' ){
        fileName.append(1,memory.at(address)) ;
        ++address;
    }

    ReadFileIntoMemoryAt(fileName.c_str(),processTable[runningProcess]->getBaseRegister());
    loadSwapToMemory();
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
        for(i=0;i<4;++i) {
            if(processTable[i]== NULL) {
                processTable[i] = new Process();
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

        uint8_t *buffer =  &hardDisk.physicalAt(processTable[i]->getBaseRegister());
        uint8_t *parentBuffer=&hardDisk.physicalAt(processTable[runningProcess]->getBaseRegister());
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
        for(i=0;i<4;++i) {
            if(processTable[i]== NULL) {
                processTable[i] = new Process();
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

        ReadFileIntoMemoryAt(processName.c_str(), 0x0000);
        //memorye okumayacağız sadece process tablea ekle process değiştiğinde oku
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
        memory.setVirtualMemoryAddressTable(processTable[runningProcess]->getVirtalMemoryAddress());
        hardDisk.setRegister(processTable[runningProcess]->getBaseRegister(),processTable[runningProcess]->getLimitRegister());
        memset(&memory.physicalAt(0),0,0x2000);
        loadSwapToMemory();
    }
}

//Ahmet Alperen BULUT
void GTUOS::changeCPUStateRegister(){
    tempCycles=cycles;
    *(cpu.state)=processTable[runningProcess]->getprocessRegister();
}

void GTUOS::saveProcess(const CPU8080 & cpu,State state,uint16_t waitPid) {
    saveMemoryToSwap();
    processTable[runningProcess]->setProcessRegister(*cpu.state);
    processTable[runningProcess]->setState(state);
    processTable[runningProcess]->setProcessUsingCycle(processTable[runningProcess]->getProcessUsingCycle()+cycles-tempCycles);
    if(state==ready)
        readyQueue.push(std::make_pair(runningProcess,processTable[runningProcess]->getPid()));
    else
        blockedQueue.push_back(std::make_pair(runningProcess,waitPid));

    processTable[runningProcess]->setVirtualMemoryAddress(memory.getVirtualMemoryAddressTable());
}

void GTUOS::saveProcess() {
    saveMemoryToSwap();
    processTable[runningProcess]->setProcessRegister(*cpu.state);
    processTable[runningProcess]->setState(ready);
    processTable[runningProcess]->setProcessUsingCycle(processTable[runningProcess]->getProcessUsingCycle()+cycles-tempCycles);
    readyQueue.push(std::make_pair(runningProcess,processTable[runningProcess]->getPid()));
    processTable[runningProcess]->setVirtualMemoryAddress(memory.getVirtualMemoryAddressTable());
}

void GTUOS::saveMemoryToSwap(){
    uint8_t *bufferMem,*bufferhardDisk;
    for(short int i=0;i<16;++i){
        if(processTable[runningProcess]->getVirtalMemoryAddress()[i].present_Absent_Bit && processTable[runningProcess]->getVirtalMemoryAddress()[i].modifiedBit){
            bufferMem=&memory.at(1024*processTable[runningProcess]->getVirtalMemoryAddress()[i].virtualPageNumber);
            bufferhardDisk= & hardDisk.at(1024*processTable[runningProcess]->getVirtalMemoryAddress()[i].virtualPageNumber);
            memcpy(bufferhardDisk,bufferMem, 1024);
            processTable[runningProcess]->getVirtalMemoryAddress()[i].modifiedBit=false;
        }
        processTable[runningProcess]->getVirtalMemoryAddress()[i].referencedBit=false;
    }

}

void GTUOS::loadSwapToMemory(){
    uint8_t *bufferMem,*bufferhardDisk;
    for(short int i=0;i<16;++i){
        if(processTable[runningProcess]->getVirtalMemoryAddress()[i].present_Absent_Bit) {
            bufferMem=&memory.physicalAt(1024*processTable[runningProcess]->getVirtalMemoryAddress()[i].pageFrameNumber);
            bufferhardDisk= & hardDisk.at(1024*processTable[runningProcess]->getVirtalMemoryAddress()[i].virtualPageNumber);
            memcpy(bufferMem,bufferhardDisk, 1024);

        }
    }
}

void GTUOS::terminateProcess(){
    int i=blockedQueue.size()-1;
    saveMemoryToSwap();
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

    for(short int i=0;i<4;++i) {
        if(processTable[i]!= NULL){
            std::printf("%-25s %-10s\n", "ProcessName", "Pid");
            std::printf("%-25s %-25d\n",processTable[i]->getProcessName().c_str(),processTable[i]->getPid());
            std::printf("%-25s %-25s %-25s %-25s %-25s\n", "Virtual Page", "Present/AbsentBit", "ReferencedBit",
                        "ModifiedBit", "Page Frame");
            for (short int j = 0; j < 16; ++j) {
                std::printf("%-25d %-25d %-25d %-25d %-25d\n", j,
                            processTable[i]->getVirtalMemoryAddress()[j].present_Absent_Bit,
                            processTable[i]->getVirtalMemoryAddress()[j].referencedBit,
                            processTable[i]->getVirtalMemoryAddress()[j].modifiedBit,
                            processTable[i]->getVirtalMemoryAddress()[j].pageFrameNumber);
            }
        }
    }
//Ahmet Alperen BULUT

}
void GTUOS::writeMemoryToFile(const std::string progName){
    std::ofstream myfile;
    int temp=0;
    std::string memName=progName.substr(0,progName.length()-4);
    memName.insert(memName.length(),".mem");
    myfile.open (memName.c_str(), std::ios_base::binary | std::ios_base::out);
    myfile  <<std::setiosflags(std::ios_base::left)<<std::setw(7)<<std::setfill(' ')<<std::dec <<"1 ";
    for(int i=0;i<0x4000;++i) {

        if(i%1024==0){
            if(!memory.isTherePage(i)){
                int virtualPage = i >> 10;
                loadPage(virtualPage);
            }
            memory.at(i);
        }
        ++temp;
        myfile <<"0x";
        myfile << std::hex << std::setfill('0') << std::setw(2) << (int16_t) memory.at(i) << " ";
        if (temp == 16) {
            myfile << std::endl << std::setw(6) << std::setfill(' ') << std::dec << i + 1 << " ";
            temp = 0;
        }
    }
    myfile.close();
}

void GTUOS::ReadFileIntoMemoryAt(const char* filename, uint32_t offset) {
    FILE *f= fopen(filename, "rb");
    if (f==NULL) {
        printf("error: Couldn't open %s\n", filename);
        exit(1);
    }
    fseek(f, 0L, SEEK_END);
    int fsize = ftell(f);
    fseek(f, 0L, SEEK_SET);

    uint8_t *buffer = &hardDisk.physicalAt(offset);
    fread(buffer, fsize, 1, f);
    fclose(f);
}

void GTUOS::pageFault(int virtualPage) {
    State8080 *buf1=&memory.recoveryState;
    State8080 *buf2=cpu.state;
    memcpy(buf2,buf1,sizeof(*buf1));
    loadPage(virtualPage);
}
void GTUOS::loadPage(int virtualPage) {
    int index=virtualPage<<10;
    int pageFrame;
    uint8_t *bufferMem,*bufferhardDisk;
    VirtualTableInformation* temp=NULL;
    if(processTable[runningProcess]->getFifoVT().size()==0){
        processTable[runningProcess]->getFifoVT().push(&memory.getVirtualMemoryAddressTable()[virtualPage]);
        pageFrame=0;
    }
    else if(processTable[runningProcess]->getFifoVT().size()==8){

            temp=processTable[runningProcess]->getFifoVT().front();
            processTable[runningProcess]->getFifoVT().pop();


                temp->referencedBit= false;

                if(temp->modifiedBit){

                    bufferMem=&memory.physicalAt(1024*temp->pageFrameNumber);
                    bufferhardDisk= & hardDisk.at(1024*temp->virtualPageNumber);
                    memcpy(bufferhardDisk,bufferMem, 1024);
                    temp->modifiedBit=false;
                }
                temp->present_Absent_Bit=false;
                processTable[runningProcess]->getFifoVT().push(&memory.getVirtualMemoryAddressTable()[virtualPage]);
                pageFrame=temp->pageFrameNumber;
    }
    else {
        processTable[runningProcess]->getFifoVT().push(&memory.getVirtualMemoryAddressTable()[virtualPage]);
        pageFrame=processTable[runningProcess]->getFifoVT().size()-1;
    }
    bufferMem=&memory.physicalAt(1024*pageFrame);
    bufferhardDisk= & hardDisk.at(index);
    memcpy(bufferMem,bufferhardDisk, 1024);
    memory.getVirtualMemoryAddressTable()[virtualPage].present_Absent_Bit=true;
    memory.getVirtualMemoryAddressTable()[virtualPage].pageFrameNumber=pageFrame;
    memory.getVirtualMemoryAddressTable()[virtualPage].referencedBit=true;

    if(debugMode==3)
        writePageFaultInformation(temp,virtualPage);
}
//Ahmet Alperen BULUT
void GTUOS::writePageFaultInformation(VirtualTableInformation* temp,int virtualPage){
    std::cout<<std::endl;
    if(temp!=NULL)
        std::cout<<std::dec<<"Degistirilen VirtualPage : "<<temp->virtualPageNumber<<" Eklenen VirtualPage :"<<virtualPage<<std::endl;
    else
        std::cout<<std::dec<<"Degistirilen VirtualPage : "<<"Degistirilen page yok !"<<" Eklenen VirtualPage :"<<virtualPage<<std::endl;

    std::cout<<"Fifo daki pageFrameler : ";
    for(int i=0;i<processTable[runningProcess]->getFifoVT().size();++i){
        temp=processTable[runningProcess]->getFifoVT().front();
        std::cout<<temp->pageFrameNumber<<"<-- ";
        processTable[runningProcess]->getFifoVT().pop();
        processTable[runningProcess]->getFifoVT().push(temp);
    }
    std::cout<<std::endl;

}