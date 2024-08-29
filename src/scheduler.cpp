//
// Created by os on 5/14/23.
//

#include "../h/scheduler.hpp"

int Scheduler::DEFAULT_PERIOD_D = 20;

bool Scheduler::isIdle = false;

TCB *Scheduler::get() {
    if(!IOblocked && IOthread)return IOthread; //mozda i neka perioda za ovo, al za sad najveci priortiet ima, ako nije blokirana odmah tu menjamo kontekst
    if(!head || periodD == 0) {
        if(!head)isIdle = true;
        periodD = DEFAULT_PERIOD_D;
        return destroyer; //destroyer ujedno i idle za sad
    }
    periodD--;

    return Queue::get();
}

void Scheduler::put(TCB *thread) {
    if(thread) isIdle = false;

    Queue::put(thread);
}
