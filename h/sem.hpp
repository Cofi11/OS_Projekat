//
// Created by os on 5/27/23.
//

#ifndef PROJEKAT_SAMOBAZA_SEM_HPP
#define PROJEKAT_SAMOBAZA_SEM_HPP

#include "queue.hpp"
#include "../lib/hw.h"

class Sem {

public:

    static Sem* closedSems;

    void* operator new(size_t size);
    void operator delete(void *ptr);


    static Sem* create(unsigned init = 1){
        return new Sem(init);
    }

    Sem*& next() {return nextSem;}

    int wait();

    int signal();

    int close();

    Sem() {} //samo za kernel dynamics

    ~Sem();

    void setUnavailable() {} //samo za kernel dynamics

protected:

    void block();

    void deblock();

private:

    Sem(unsigned init): val(init), closed(false), nextSem(nullptr) {}

    int val;
    Queue blocked;
    bool closed;

    Sem* nextSem;

};


#endif //PROJEKAT_SAMOBAZA_SEM_HPP
