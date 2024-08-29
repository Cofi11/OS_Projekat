//
// Created by os on 4/29/23.
//

#include "../h/riscv.hpp"
#include "../h/memoryAllocator.hpp"
#include "../h/tcb.hpp"
#include "../h/kernelDynamics.hpp"
#include "../h/sem.hpp"
#include "../h/cons.hpp"

int Riscv::cnt = 0;

void Riscv::handleSupervisorTrap() {
    //OVDE SVE a0..a5 da procitam da bi bile na steku jer se mzd registri posle promene
    uint64 volatile a5 = read_a5();
    uint64 volatile a4 = read_a4();
    uint64 volatile a3 = read_a3();
    uint64 volatile a2 = read_a2();
    uint64 volatile a1 = read_a1();
    uint64 volatile a0 = read_a0();

    uint64 scause = read_scause();

    if(scause == 0x0000000000000008 || scause == 0x0000000000000009 ){
        //ecall iz U ili S moda
        uint64 volatile sepc = read_sepc() + 4;//volatile da sepc bude na steku
        uint64 volatile sstatus = read_sstatus();
        //....

        if(a0 == 1){//mem_alloc
            MemoryAllocator &m = MemoryAllocator::create();
            void* adr = m.mem_alloc(a1);
            write_a0_onStack((uint64)adr); //da bi pri restauriranju konteksta to bila prava povratna vrednost
        }
        else if(a0 == 2){//mem_free
            MemoryAllocator &m = MemoryAllocator::create();
            int ret = m.mem_free((void*)a1);
            write_a0_onStack(ret);
        }
        else if(a0 == 0x11){//thread_create
            uint64* handle = (uint64*) a1;
            void (*f) (void*) = (void (*) (void*)) a2;
            void *a = (void*) a3;
            uint64* st = (uint64*) a4;
            uint64* kst = (uint64*) a5;

            TCB* thread = TCB::create(f,a,st,kst);

            if(!thread)write_a0_onStack(-1);
            else{
                *handle = thread->getID();
                write_a0_onStack(0);
            }
        }
        else if(a0 == 0x12){//thread_exit
            TCB::running->setCompleted(true);
            TCB::yield();
            //ne deluje mi da ikad moze greska da se desi??
            write_a0_onStack(0);
        }
        else if(a0 == 0x13){//thread_dispatch
            //TCB::timeSpent = 0;
            TCB::yield();
        }
        else if(a0 == 0x14){//thread join
            uint64 threadID =  a1;
            //pretraga po ID dal je pozivajuca nit jos uvek medju aktivnima i samo tad se tekuca suspenduje tj join uradi
            KernelDynamics<TCB> &kd = KernelDynamics<TCB>::create();
            TCB* thread = kd.findById(threadID);
            if(thread)thread->join();
        }
        else if(a0 == 0x21){//sem open
            Sem** handle = (Sem**)a1;
            unsigned init = (unsigned)a2;
            Sem* sem = Sem::create(init);

            if(!sem)write_a0_onStack(-1);
            else{
                *handle = sem;
                write_a0_onStack(0);
            }
        }
        else if(a0 == 0x22){//sem close
            Sem* sem = (Sem*)a1;
            if(!sem)write_a0_onStack(-1);
            else {
                int ret = sem->close();
                write_a0_onStack(ret);
            }
        }
        else if(a0 == 0x23){//sem wait
            Sem* sem = (Sem*)a1;
            if(!sem)write_a0_onStack(-1);
            else {
                int ret = sem->wait();
                write_a0_onStack(ret);
            }
        }
        else if(a0 == 0x24){//sem signal
            Sem* sem = (Sem*)a1;
            if(!sem)write_a0_onStack(-1);
            else {
                int ret = sem->signal();
                write_a0_onStack(ret);
            }
        }
        else if(a0 == 0x31){//sleep
            time_t t = (time_t)a1;
            if(t<0)write_a0_onStack(-1);
            else {
                if(t>0)
                    TCB::sleep(t); //doda nit u uspavanu listu i yielduje
                write_a0_onStack(0);
            }
        }
        else if(a0 == 0x41){//getc
            Cons &c = Cons::create();
            char ch = c.getc();
            write_a0_onStack(ch);
        }
        else if(a0 == 0x42){//putc
            char ch = (char)a1;
            Cons &c = Cons::create();
            c.putc(ch);
        }

        //...
        write_sstatus(sstatus);
        write_sepc(sepc);
    }

    /*else if(scause == 0x8000000000000001){
        // tajmer prekid
        //Cons &c = Cons::create();
        //c.putc('W');
        clear_sip(SSIP);
        TCB::timeSpent ++;
        if(TCB::timeSpent == TCB::running->getTimeSlice()){
            uint64 volatile sepc = read_sepc();
            uint64 volatile sstatus = read_sstatus();
            //TCB::timeSpent = 0;
            TCB::yield();
            write_sstatus(sstatus);
            write_sepc(sepc);
        }
        TCB::wakeUp();
    }
    else if(scause == 0x8000000000000009){
        //spoljasnji hardverski
        //console_handler();
        int irq = plic_claim();
        if(irq == CONSOLE_IRQ) {
            Cons &c = Cons::create();
            while (*((char *) CONSOLE_STATUS) & CONSOLE_RX_STATUS_BIT) {
                char ch = *((char *) CONSOLE_RX_DATA);
                c.putInIput(ch);
            }
            plic_complete(irq);
        }
    }*/
    else{
        //neocekivano
    }

}


void Riscv::handleConsole() {
    //spoljasnji hardverski
    int irq = plic_claim();
    if(irq == CONSOLE_IRQ) {
        Cons &c = Cons::create();
        while (*((char *) CONSOLE_STATUS) & CONSOLE_RX_STATUS_BIT) {
            char ch = *((char *) CONSOLE_RX_DATA);
            c.putInIput(ch);
        }
        plic_complete(irq);
    }
}

void Riscv::handleTimer() {
    //tajmer prekid
    clear_sip(SSIP);
    TCB::timeSpent ++;
    if(TCB::timeSpent == TCB::running->getTimeSlice()){
        uint64 volatile sepc = read_sepc();
        uint64 volatile sstatus = read_sstatus();
        //TCB::timeSpent = 0;
        TCB::yield();
        write_sstatus(sstatus);
        write_sepc(sepc);
    }
    TCB::wakeUp();

}

void Riscv::sretPrevMode(bool kernel) {
    if(kernel){
        set_sstatus(SPP);
    }
    else{
        clear_sstatus(SPP);
    }
    asm volatile("csrw sepc, ra");
    asm volatile("sret");
}

void Riscv::lock() {
    cnt++;
    clear_sstatus(SIE);
}

void Riscv::unlock() {
    cnt--;
    if(cnt<=0){
        cnt = 0;
        set_sstatus(SIE);
    }
}
