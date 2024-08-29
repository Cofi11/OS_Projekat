//
// Created by os on 5/30/23.
//

#include "../h/cons.hpp"
#include "../h/scheduler.hpp"

void Cons::putc(char c) {
    outputReady->wait();
    cntOut++;
    outputbuf[tailOut] = c;
    tailOut = (tailOut + 1) % SIZE_BUF;
    Scheduler &s = Scheduler::create();
    s.setIOBlocked(false);
}

char Cons::getFromOutput() {
    if(cntOut == 0)return -1;
    cntOut--;
    char ch = outputbuf[headOut];
    headOut = (headOut + 1) % SIZE_BUF;
    outputReady->signal();
    return ch;
}



char Cons::getc() {
    inputReady->wait();
    cntIn--;
    char ch = inputBuf[headIn];
    headIn = (headIn + 1) % SIZE_BUF;
    return ch;
}

void Cons::putInIput(char c) {
    if(cntIn == SIZE_BUF)return;
    cntIn++;
    inputBuf[tailIn] = c;
    tailIn = (tailIn + 1) % SIZE_BUF;
    inputReady->signal();
}


