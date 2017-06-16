//
// Created by Ahmet Alperen BULUT on 24.05.2017.
//

#ifndef PAGEFAULTEXCEPTION_H
#define PAGEFAULTEXCEPTION_H

#include <iostream>
#include <string>

class PageFaultException {
private:
    std::string m_error;
    int pageFrame;
public:

    PageFaultException(std::string error,int pageFrame) :m_error(error),pageFrame(pageFrame){}
    std::string getError() { return m_error; }
    int getPageFrame(){return pageFrame;}
};
#endif //PAGEFAULTEXCEPTION_H
