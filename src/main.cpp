//
// Created by os on 4/22/23.
//
#include "../h/memoryAllocator.hpp"
#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../h/syscall_c.hpp"
#include "../h/sem.hpp"

extern void userMain();

void userMainWrapper(void *){
    userMain();
}

int main(){
    Riscv::write_stvec((uint64) &Riscv::supervisorTrap + 1);

    size_t size = (DEFAULT_STACK_SIZE * 2 - 1) / MEM_BLOCK_SIZE + 1; //kernel stek manji 4 * puta, DEFAULT_STACK_SIZE / 4 podataka velicine 8 bajtova

    uint64 *kst1 = (uint64*)MemoryAllocator::create().mem_alloc(size);

    TCB* t_kmain = TCB::create(nullptr,nullptr,nullptr,kst1 + DEFAULT_STACK_SIZE / 4); //ne postavljamo kernle mode na true, jer nam ni ne treba za ovu nit, to sluzi za ostale kernel niti
    TCB::running = t_kmain;

    uint64 *kst2 = (uint64*)MemoryAllocator::create().mem_alloc(size);
    size = (DEFAULT_STACK_SIZE * 8 - 1) / MEM_BLOCK_SIZE + 1; //velicina user steka u blokovima
    uint64 *stack2 = (uint64*)MemoryAllocator::create().mem_alloc(size);

    uint64 *stack3 = (uint64*)MemoryAllocator::create().mem_alloc(size);
    size = (DEFAULT_STACK_SIZE * 2 - 1) / MEM_BLOCK_SIZE + 1;  //velicina kernel steka u blokovima
    uint64 *kst3 = (uint64*)MemoryAllocator::create().mem_alloc(size);


    TCB* t_umain = TCB::create(&userMainWrapper,nullptr,stack2 + DEFAULT_STACK_SIZE, kst2 + DEFAULT_STACK_SIZE / 4);//user thread by default, kernel = false

    TCB* t_destroyer = TCB::create(&TCB::threadDestroyer, nullptr, stack3 + DEFAULT_STACK_SIZE, kst3 + DEFAULT_STACK_SIZE / 4, true);//kernel nit

    size = (DEFAULT_STACK_SIZE * 8 - 1) / MEM_BLOCK_SIZE + 1; //velicina user steka u blokovima
    uint64 *stack4 = (uint64*)MemoryAllocator::create().mem_alloc(size);
    size = (DEFAULT_STACK_SIZE * 2 - 1) / MEM_BLOCK_SIZE + 1;  //velicina kernel steka u blokovimaa
    uint64 *kst4 = (uint64*)MemoryAllocator::create().mem_alloc(size);

    TCB* IOthread = TCB::create(&TCB::IOthread,nullptr,stack4 + DEFAULT_STACK_SIZE, kst4 + DEFAULT_STACK_SIZE / 4, true);

    Scheduler &s = Scheduler::create();
    s.setDestroyer(t_destroyer);
    s.setIOthread(IOthread);

    thread_join(t_umain->getID());

    s.setPeriodD(0);//da se osiguramo da ce skociti na destroyer dispatch
    TCB::dispatch();//da skoci na destroyer za slucaj da nije sve unisteno

    delete t_destroyer;
    delete IOthread;

    //brisemo semafore tek na kraju
    while(Sem::closedSems){
        Sem* old = Sem::closedSems;
        Sem::closedSems = old->next();
        delete old;
    }

    MemoryAllocator::create().mem_free(kst1);

    t_kmain->setCompleted(true);

    return 0;
}