//
// Created by os on 4/30/23.
//

#include "../h/syscall_c.hpp"

sem_t semafor1, semafor2;

int fibonaci(int n){
    if(n == 0)return 0;
    if(n == 1)return 1;
    //if(n % 4 == 0)thread_dispatch();
    return fibonaci(n-1) + fibonaci(n-2);
}

void fun(void *){
    int i=10;

    getc();

    while(1){
        //putc('D');
        //sem_wait(semafor1);
        int fib = fibonaci(i);
        //thread_dispatch();
        for(int j=0 ; j<fib ; j++){
            //for(int k=0 ; k<10000000 ; k++);
            putc('F');
        }
        putc('\n');
        time_sleep(6);
        //sem_signal(semafor2);
        if(--i == 0)thread_exit();
    }
}

void fun2(void *){
    int i=10;

    while(1){

        //putc('E');
        //sem_wait(semafor2);
        int fib = fibonaci(i);
        //thread_dispatch();
        for(int j=0 ; j<fib ; j++){
            //for(int k=0 ; k<10000000 ; k++);
            putc('f');
        }
        putc('\n');
        time_sleep(20);
        //sem_signal(semafor1);
        if(--i == 0)thread_exit();
    }
}

void fun3(void*){
    for(int i=0 ; i<40 ; i++){
        putc('A');
        time_sleep(3);
    }
}

void fun4(void*){
    for (uint64 i = 0; i < 10; i++) {
        putc('A');
        //putc('\n');
        for (uint64 j = 0; j < 10000; j++) {
            for (uint64 k = 0; k < 30000; k++) { /* busy wait */ }
            thread_dispatch();
        }
    }
}

void fun5(void*){
    for (uint64 i = 0; i < 16; i++) {
        putc('B');
        //putc('\n');
        for (uint64 j = 0; j < 10000; j++) {
            for (uint64 k = 0; k < 30000; k++) { /* busy wait */ }
            thread_dispatch();
        }
    }
}



static volatile bool finishedA = false;
static volatile bool finishedB = false;
static volatile bool finishedC = false;
static volatile bool finishedD = false;

static uint64 fibonacci(uint64 n) {
    if (n == 0 || n == 1) { return n; }
    if (n % 10 == 0) { thread_dispatch(); }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

static void workerBodyA2(void* arg) {
    for (uint64 i = 0; i < 10; i++) {
        putc('A');
        //printString("A: i="); printInt(i); printString("\n");
        for (uint64 j = 0; j < 10000; j++) {
            for (uint64 k = 0; k < 30000; k++) { /* busy wait */ }
            thread_dispatch();
        }
    }
    //printString("A finished!\n");
    finishedA = true;
}

static void workerBodyB2(void* arg) {
    for (uint64 i = 0; i < 16; i++) {
        putc('B');
        //printString("B: i="); printInt(i); printString("\n");
        for (uint64 j = 0; j < 10000; j++) {
            for (uint64 k = 0; k < 30000; k++) { /* busy wait */ }
            thread_dispatch();
        }
    }
    //printString("B finished!\n");
    finishedB = true;
    thread_dispatch();
}

static void workerBodyC2(void* arg) {
    uint8 i = 0;
    for (; i < 3; i++) {
        //printString("C: i="); printInt(i); printString("\n");
    }

    //printString("C: dispatch\n");
    __asm__ ("li t1, 7");
    thread_dispatch();

    uint64 t1 = 0;
    __asm__ ("mv %[t1], t1" : [t1] "=r"(t1));

    //printString("C: t1="); printInt(t1); printString("\n");

    /*uint64 result = */fibonacci(12);
    //printString("C: fibonaci="); printInt(result); printString("\n");

    for (; i < 6; i++) {
        //printString("C: i="); printInt(i); printString("\n");
    }

    //printString("A finished!\n");
    finishedC = true;
    thread_dispatch();
}

static void workerBodyD2(void* arg) {
    uint8 i = 10;
    for (; i < 13; i++) {
        //printString("D: i="); printInt(i); printString("\n");
    }

    //printString("D: dispatch\n");
    __asm__ ("li t1, 5");
    thread_dispatch();

    /*uint64 result = */fibonacci(16);
    //printString("D: fibonaci="); printInt(result); printString("\n");

    for (; i < 16; i++) {
        //printString("D: i="); printInt(i); printString("\n");
    }

    //printString("D finished!\n");
    finishedD = true;
    thread_dispatch();
}


void Threads_C_API_test2() {
    thread_t threads[4];
    thread_create(&threads[0], workerBodyA2, nullptr);
    //printString("ThreadA created\n");

    thread_create(&threads[1], workerBodyB2, nullptr);
    //printString("ThreadB created\n");

    thread_create(&threads[2], workerBodyC2, nullptr);
    //printString("ThreadC created\n");

    thread_create(&threads[3], workerBodyD2, nullptr);
    //printString("ThreadD created\n");

    while (!(finishedA && finishedB && finishedC && finishedD)) {
        thread_dispatch();
    }

}



void userMain2(){

    //sem_open(&semafor1, 1);
    //sem_open(&semafor2, 0);

    /*char ch;
    while((ch = getc()) != 'S'){
        putc(ch);
    }
    getc(); //da pokupi enter ako ne pokrenem iz terminala*/

    for(int i=0 ; i< 500 ; i++)putc('I');
    putc('\n');

    /*thread_t handle;
    thread_create(&handle, &fun, nullptr);
    thread_t handle2;
    thread_create(&handle2, &fun2, nullptr);
    thread_t handle3;
    thread_create(&handle3, &fun3, nullptr);*/

    /*thread_t handle;
    thread_create(&handle, &fun4, nullptr);
    thread_t handle2;
    thread_create(&handle2, &fun5, nullptr);

    //thread_dispatch();
    //thread_dispatch();


    thread_join(handle);
    thread_join(handle2);*/
    //thread_join(handle3);

    //sem_close(semafor1);
    //sem_close(semafor2);


    Threads_C_API_test2();

}

