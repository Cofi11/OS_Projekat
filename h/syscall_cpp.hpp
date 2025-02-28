//
// Created by os on 5/30/23.
//

#ifndef PROJEKAT_SAMOBAZA_SYSCALL_CPP_HPP
#define PROJEKAT_SAMOBAZA_SYSCALL_CPP_HPP

#include "syscall_c.hpp"

void* operator new (size_t s);
void operator delete (void* ptr);

class Thread {

public:

    Thread (void (*body)(void*), void* arg);
    virtual ~Thread ();
    int start ();
    void join();
    static void dispatch ();
    static int sleep (time_t);

protected:

    Thread ();
    virtual void run () {}

private:

    static void runWrapper(void *);

    thread_t myHandle;
    void (*body)(void*); void* arg;
};

class Semaphore {

public:

    Semaphore (unsigned init = 1);
    virtual ~Semaphore ();
    int wait ();
    int signal ();

private:

    sem_t myHandle;
};

class PeriodicThread : public Thread {

public:

    void terminate ();

protected:

    PeriodicThread (time_t period);
    virtual void periodicActivation () {}

private:

    static void periodicActivationWrapper(void *t);

    time_t period;
};

class Console {

public:

    static char getc ();

    static void putc (char);
};


#endif //PROJEKAT_SAMOBAZA_SYSCALL_CPP_HPP
