//
// Created by os on 5/27/23.
//

#include "../h/sem.hpp"
#include "../h/scheduler.hpp"
#include "../h/tcb.hpp"
#include "../h/kernelDynamics.hpp"

Sem* Sem::closedSems = nullptr;

int Sem::wait() {
    if(closed)return -1;
    if(--val < 0)block();
    if(closed) return -1; //ako je zatvoren dok je bio blokiran
    return 0;
}

int Sem::signal() {
    if(closed)return -1;
    if(val++ < 0)deblock();
    return 0;
}

void Sem::block() {
    Scheduler &s = Scheduler::create();
    TCB* old = TCB::running;
    blocked.put(old);
    TCB::running = s.get();
    TCB::timeSpent = 0;
    TCB::yield(&old->context, &TCB::running->context);
}

void Sem::deblock() {
    TCB* t = blocked.get();
    Scheduler &s = Scheduler::create();
    s.put(t);
}

void *Sem::operator new(size_t size) {
    KernelDynamics<Sem> &kd = KernelDynamics<Sem>::create();
    return kd.getSpace();
}

void Sem::operator delete(void *ptr) {
    KernelDynamics<Sem> &kd = KernelDynamics<Sem>::create();
    kd.freeSpace(ptr);
}

Sem::~Sem() {
    while(!blocked.empty()){
        deblock();
    }
}

int Sem::close() {
    if(closed)return -1;

    //dodajemo u listu zatvorenih
    nextSem = closedSems;
    closedSems = this;

    closed = true;
    while(!blocked.empty()){
        deblock();
    }
    return 0;
}
