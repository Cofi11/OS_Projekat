//
// Created by os on 5/9/23.
//

#include "../h/syscall_c.hpp"
#include "../h/riscv.hpp"

void* mem_alloc(size_t size){

    size = (size - 1) / MEM_BLOCK_SIZE + 1;
    Riscv::write_a1(size);
    Riscv::write_a0(0x01);

    asm volatile("ecall");

    uint64 adr = Riscv::read_a0();
    return (void*)adr;

}

int mem_free(void *ptr){

    Riscv::write_a1((uint64)ptr);
    Riscv::write_a0(0x02);

    asm volatile("ecall");

    int ret = Riscv::read_a0();
    return ret;

}

int syscaller_thread_create(uint64 a0, uint64 a1, uint64 a2, uint64 a3, uint64 a4, uint64 a5){
    asm volatile("ecall");

    int ret = Riscv::read_a0();
    return ret;
}

int thread_create(thread_t* handle, void (*start_routine) (void*), void* arg){

    if(!handle)return -1;
    if(!start_routine)return -2;

    uint64 *ust = (uint64*) mem_alloc(DEFAULT_STACK_SIZE * 8);
    if(!ust)return -3;
    uint64 *kst = (uint64*) mem_alloc(DEFAULT_STACK_SIZE * 2);
    if(!kst){
        mem_free(ust);
        return  -4;
    }

   return syscaller_thread_create(0x11, (uint64)handle, (uint64)start_routine, (uint64)arg,
                                  (uint64)(ust + DEFAULT_STACK_SIZE), (uint64)(kst + DEFAULT_STACK_SIZE/4));
}

int thread_exit(){

    Riscv::write_a0(0x12);

    asm volatile("ecall");

    int ret = Riscv::read_a0();
    return ret;

}


void thread_dispatch(){

    Riscv::write_a0(0x13);

    asm volatile("ecall");

}

void thread_join(thread_t handle){
    //if(handle != -1) { hadnle je samo ID, a ako je neki nepostojeci svkk se nsita nece desiti u obradi syscall
        Riscv::write_a1(handle);
        Riscv::write_a0(0x14);

        asm volatile("ecall");
    //}

}


int sem_open(sem_t* handle, unsigned init){
    Riscv::write_a2((uint64)init);
    Riscv::write_a1((uint64)handle);
    Riscv::write_a0(0x21);

    asm volatile("ecall");

    int ret = Riscv::read_a0();
    return ret;
}

int sem_close(sem_t handle){
    Riscv::write_a1((uint64)handle);
    Riscv::write_a0(0x22);

    asm volatile("ecall");

    int ret = Riscv::read_a0();
    return ret;
}

int sem_wait(sem_t id){
    Riscv::write_a1((uint64)id);
    Riscv::write_a0(0x23);

    asm volatile("ecall");

    int ret = Riscv::read_a0();
    return ret;
}

int sem_signal(sem_t id){
    Riscv::write_a1((uint64)id);
    Riscv::write_a0(0x24);

    asm volatile("ecall");

    int ret = Riscv::read_a0();
    return ret;
}

int time_sleep(time_t t){
    Riscv::write_a1((uint64)t);
    Riscv::write_a0(0x31);

    asm volatile("ecall");

    int ret = Riscv::read_a0();
    return ret;
}

char getc(){
    Riscv::write_a0(0x41);

    asm volatile("ecall");

    char ret = (char)Riscv::read_a0();
    return ret;
}

void putc(char ch){
    Riscv::write_a1((uint64)ch);
    Riscv::write_a0(0x42);

    asm volatile("ecall");
}