//
// Created by os on 5/14/23.
//

#ifndef PROJEKAT_SAMOBAZA_SCHEDULER_HPP
#define PROJEKAT_SAMOBAZA_SCHEDULER_HPP

#include "../h/queue.hpp"

class TCB;

class Scheduler : public  Queue{

public:

    static Scheduler& create(){
        static Scheduler scheduler;
        return scheduler;
    }

    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;

    TCB* get();

    void put(TCB* thread);

    void setDestroyer(TCB* des) {destroyer = des;}

    void setIOthread(TCB* iot) {IOthread = iot;}

    void setIOBlocked(bool b) {IOblocked = b;}

    void setPeriodD(int pd) {periodD = pd;}

    static bool isIdle;

private:

    TCB* destroyer;

    TCB* IOthread;
    bool IOblocked;

    int periodD;//perioda za destroyer
    static int DEFAULT_PERIOD_D;

    Scheduler():Queue(), destroyer(nullptr), IOthread(nullptr), IOblocked(true), periodD(DEFAULT_PERIOD_D) {}

};


#endif //PROJEKAT_SAMOBAZA_SCHEDULER_HPP
