//
// Created by os on 5/9/23.
//

#ifndef PROJEKAT_SAMOBAZA_SYSCALL_C_HPP
#define PROJEKAT_SAMOBAZA_SYSCALL_C_HPP

#include "../lib/hw.h"

void* mem_alloc(size_t);

int mem_free(void*);

//class TCB;
typedef uint64 thread_t;

int thread_create(thread_t* handle, void (*start_routine) (void*), void* arg);

void thread_dispatch();

int thread_exit();

void thread_join(thread_t handle);

class Sem;
typedef Sem* sem_t;

int sem_open(sem_t* handle, unsigned init);

int sem_close(sem_t handle);

int sem_wait(sem_t id);

int sem_signal(sem_t id);

int time_sleep(time_t t);

const int EOF = -1;

char getc();

void putc(char ch);

#endif //PROJEKAT_SAMOBAZA_SYSCALL_C_HPP
