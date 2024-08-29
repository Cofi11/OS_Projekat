//
// Created by os on 5/13/23.
//

#include "../h/tcb.hpp"
#include "../h/memoryAllocator.hpp"
#include "../h/kernelDynamics.hpp"
#include "../h/cons.hpp"

TCB* TCB::running = nullptr;
TCB* TCB::finishedHead = nullptr;
TCB* TCB::sleepingHead = nullptr;
time_t TCB::timeSpent = 0;
uint64 TCB::nextid = 1;

void *TCB::operator new(size_t size) {
    KernelDynamics<TCB> &kd = KernelDynamics<TCB>::create();
    return kd.getSpace();
}

void TCB::operator delete(void *ptr) {
    KernelDynamics<TCB> &kd = KernelDynamics<TCB>::create();
    kd.freeSpace(ptr);
}

TCB* TCB::create(FUN f, void *a, uint64 *st, uint64 *kst, bool kernel){
    return new TCB(f, a, st, kst, kernel); // operator new preklopljen
}

void TCB::threadWrapper(void *) {
    Riscv::sretPrevMode(running->kernel_mode);
    running->fun(TCB::running->arg);
    running->setCompleted(true);
    //unutar dispatcha ce pozvati yield i tu videti da je old completed pa ce ga dodati u finishedQueue sto threadDestroyer obradjuje

    dispatch();

}

void TCB::dispatch() {
    asm volatile("li a0, 0x13");//kao syscall za dispatch, samo radi yield()
    asm volatile("ecall");
}

void TCB::yield() {

    timeSpent = 0;
    Scheduler &s = Scheduler::create();
    TCB* old = running;
    if(!old->isFinished() && !old->kernel_mode){ //kernel niti, tj trenutno Destroyer i IOthread, ne ubacujemo u obican scheduler
        s.put(old);
    }
    else if(old->isFinished()){
        if(old->parent->waitingFor == old){//ako je neka nit cekala bas na ovu
            s.put(old->parent);
            old->parent->waitingFor = nullptr;
        }

        //SALJE SE threadDestroyeru da ga on dealocira kad bude preuzeo procesor
        old->nextInQueue = finishedHead;
        finishedHead = old;
    }
    running = s.get();

    if(old != running)
        yield(&old->context, &running->context);

}

TCB::~TCB() {

    MemoryAllocator &m = MemoryAllocator::create();
    if(user_stack)
        m.mem_free(user_stack);
    if(kernel_stack)
        m.mem_free(kernel_stack);

}

void TCB::join() {

    if(this->isFinished() || this->parent != running)return;

    parent->waitingFor = this;

    Scheduler &s = Scheduler::create();
    TCB* old = running;
    running = s.get();
    timeSpent = 0;

    yield(&old->context, &running->context);

}

void TCB::sleep(time_t t) {

    time_t sum = 0;
    time_t oldSum = 0;
    TCB* temp = sleepingHead, *prev = nullptr;

    while(temp){
        oldSum = sum;
        sum += temp->getRelativeSleep();
        if(sum >= t)break;
        prev = temp;
        temp = temp->next();
    }

    TCB* old = running;
    old->next() = temp;
    if(!prev) {
        sleepingHead = old;
    }
    else{
        prev->next() = old;
    }
    if(temp){
        old->setRelativeSleep(t - oldSum);
        temp->setRelativeSleep(sum - t);
    }
    else{
        old->setRelativeSleep(t - sum);
    }

    Scheduler &s = Scheduler::create();
    running = s.get();
    timeSpent = 0;

    yield(&old->context, &running->context);


}

void TCB::wakeUp() {

    if(!sleepingHead)return;
    sleepingHead->setRelativeSleep(sleepingHead->getRelativeSleep() - 1);
    TCB* temp = sleepingHead;
    Scheduler &s = Scheduler::create();
    while(temp && temp->getRelativeSleep() == 0){
        TCB* old = temp;
        temp = temp->next();
        s.put(old);
    }
    sleepingHead = temp;

}

void TCB::threadDestroyer(void *){
    while(1) {
        TCB *head = finishedHead;
        while (head) {
            TCB *old = head;
            head = head->next();
            finishedHead = head;
            /*Cons &c = Cons::create();
            c.putc('D');
            c.putc('e');
            c.putc('l');*/
            delete old;
        }
        Riscv::unlock();//samo da bi i odavde mogao da ode u tajmer i budi niti
        while(Scheduler::isIdle);
        Riscv::lock();
        dispatch();
    }
}

void TCB::IOthread(void *) {

    while(1) {
        Cons &c = Cons::create();
        while (*((char *) CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT) {
            char ch = c.getFromOutput();
            if (ch == char(-1))break;
            *((char *) CONSOLE_TX_DATA) = ch;
        }

        Scheduler &s = Scheduler::create();
        s.setIOBlocked(true);
        dispatch();
    }
}
