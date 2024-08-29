//
// Created by os on 5/30/23.
//

#ifndef PROJEKAT_SAMOBAZA_CONS_HPP
#define PROJEKAT_SAMOBAZA_CONS_HPP

#include "../lib/hw.h"

#include "../h/sem.hpp"

class Cons {

public:

    static Cons& create(){
        static Cons cons;
        return cons;
    }

    Cons(const Cons&) = delete;
    Cons& operator=(const Cons&) = delete;


    void putc(char c);

    char getFromOutput();

    char getc();

    void putInIput(char c);

private:

    static const uint16 SIZE_BUF = 256;

    char inputBuf[SIZE_BUF], outputbuf[SIZE_BUF];
    uint16 headIn, tailIn, headOut, tailOut;
    uint16 cntIn, cntOut;

    Sem* outputReady, *inputReady;

    Cons() : headIn(0), tailIn(0), headOut(0), tailOut(0), cntIn(0), cntOut(0){
        outputReady = Sem::create(SIZE_BUF);
        inputReady = Sem::create(0);
    }

};


#endif //PROJEKAT_SAMOBAZA_CONS_HPP
