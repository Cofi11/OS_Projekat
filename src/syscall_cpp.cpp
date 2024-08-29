//
// Created by os on 5/31/23.
//

#include "../h/syscall_cpp.hpp"

void Thread::runWrapper(void *t){
    Thread* thread = (Thread*) t;
    thread->run();
}

void* operator new(size_t s) {

    return mem_alloc(s);

}

void operator delete(void *ptr){

    mem_free(ptr);

}


Thread::Thread(void (*body)(void *), void *arg) {

    this->body = body;
    this->arg = arg;
    this->myHandle = -1;

}

Thread::Thread() {

    this->body = nullptr;
    this->myHandle = -1;

}

int Thread::start() {

    if(this->myHandle == (thread_t)-1){
        if(this->body){
            thread_create(&this->myHandle, this->body, this->arg);
        }
        else{
            thread_create(&this->myHandle,&Thread::runWrapper, (void*) this);
        }
        return 0;
    }
    return -1;

}

Thread::~Thread(){

    thread_join(this->myHandle); //MOZDA MOZE BEZ join ako racunamo da korinsik nece brisati aktivne niti?

}

void Thread::join() {

    thread_join(myHandle);

}

void Thread::dispatch() {

    thread_dispatch();

}

int Thread::sleep(time_t t) {

    int ret = time_sleep(t);
    return ret;

}

Semaphore::Semaphore(unsigned int init) {

    sem_open(&myHandle, init);

}

Semaphore::~Semaphore() {

    sem_close(myHandle);

}

int Semaphore::wait() {

    int ret = sem_wait(myHandle);
    return ret;

}

int Semaphore::signal() {

    int ret = sem_signal(myHandle);
    return ret;

}

PeriodicThread::PeriodicThread(time_t period) : Thread(&PeriodicThread::periodicActivationWrapper, (void*)this), period(period){}

void PeriodicThread::terminate() {

    period = -1;

}

void PeriodicThread::periodicActivationWrapper(void *t) {
    PeriodicThread *pt = (PeriodicThread*) t;
    while(pt->period != (time_t)-1){
        pt->periodicActivation();
        time_sleep(pt->period);
    }
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char ch) {
    ::putc(ch);
}
