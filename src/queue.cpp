//
// Created by os on 5/27/23.
//

#include "../h/queue.hpp"
#include "../h/tcb.hpp"

void Queue::put(TCB *thread) {

    if(!thread)return;
    thread->next() = nullptr; //ubacujemo na kraj
    if(!head)head = thread;
    if(last)last->next() = thread;
    last = thread;

}

TCB *Queue::get() {
    if(!head)return nullptr;
    TCB* ret = head;
    head = head->next();
    ret->next() = nullptr;
    if(!head)last = nullptr;
    return ret;
}