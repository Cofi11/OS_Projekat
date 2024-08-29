//
// Created by os on 5/13/23.
//

#ifndef PROJEKAT_SAMOBAZA_TCB_HPP
#define PROJEKAT_SAMOBAZA_TCB_HPP

#include "../lib/hw.h"
#include "riscv.hpp"
#include "scheduler.hpp"

class TCB {

public:
    void* operator  new(size_t size);
    void operator delete(void *ptr);

    static TCB* running;
    static TCB* finishedHead;
    static time_t timeSpent;
    static TCB* sleepingHead;

    using FUN = void (*) (void*);

    static TCB* create(FUN f, void *a, uint64 *st, uint64 *kst, bool kernel = false);

    TCB() : user_stack(nullptr), kernel_stack(nullptr), nextInQueue(nullptr) {} //da moze da popuni inicijalno niz u pregradama, i da zna da u destrkutoru to ne dealocira
    TCB(const TCB&) = delete;
    TCB& operator=(const TCB&) = delete;

    ~TCB();

    bool isFinished() const {return completed;}
    void setCompleted(bool c) { completed = c;}

    void setUnavailable() {user_stack = kernel_stack = nullptr;} // to kad se freeuje prava nit u chunku, da setujemo onu "nit" koja ostane da destruktor zna da ne dealocira stekove

    uint64 getID() const {return id;}

    time_t getTimeSlice() const {return time_slice;}

    time_t getRelativeSleep() const {return relativeSleep;}
    void setRelativeSleep(time_t t) {relativeSleep = t;}

    TCB*& next() {return  nextInQueue;}

    static void dispatch();

    static void yield();//il javno il da Riscv bude prijatelj

    void join();

    static void sleep(time_t t);

    static void wakeUp();

    static void threadDestroyer(void *);

    static void IOthread(void *);

private:
    TCB(FUN f, void *a, uint64* st, uint64* kst, bool kernel): fun(f), arg(a), user_stack(st - DEFAULT_STACK_SIZE), //st ukazuje na poslednju zauzetu
            kernel_stack(kst - DEFAULT_STACK_SIZE / 4), time_slice(DEFAULT_TIME_SLICE), nextInQueue(nullptr),
            completed(false), kernel_mode(kernel),
            parent(running), waitingFor(nullptr), relativeSleep(0){
        sp = (uint64) st;
        ssp = (uint64) kst;
        context.sp = (uint64) st;
        context.ra = (uint64) &threadWrapper;
        id = nextid++;
        //context.k_mode = false; //vratice se u user mode

        //ubacivanje u scheduler
        if(f && !kernel) { //za kernel niti, posebno dodajemo u scheduler, jer zelimo da ih drugacije rasporedjujemo
            Scheduler &s = Scheduler::create();
            s.put(this);
        }
    }

    FUN fun;
    void* arg;
    uint64 sp;
    uint64 ssp;
    uint64* user_stack;
    uint64* kernel_stack;
    time_t time_slice;

    struct Context{
        uint64 sp;
        uint64 ra;
        //bool k_mode;
    };
    Context context;

    TCB* nextInQueue; //za scheduler i blocked na semaforima isti red jer nemoguce da je jedna nit u oba
    //TCB* sleepingList; //to ce mi mzd trebati

    bool completed;
    bool kernel_mode;

    TCB* parent;
    TCB* waitingFor;

    uint64 id;

    time_t relativeSleep;

    static uint64 nextid;


    static void threadWrapper(void *);

    static void yield(Context* oldContext, Context* newContext);

    friend class Sem;

};


#endif //PROJEKAT_SAMOBAZA_TCB_HPP
