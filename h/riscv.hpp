//
// Created by os on 4/29/23.
//

#ifndef PROJEKAT_SAMOBAZA_RISCV_HPP
#define PROJEKAT_SAMOBAZA_RISCV_HPP

#include "../lib/hw.h"

class Riscv {

public:

    enum SSTATUS_BITS{
        SPP = 1<<8,
        SIE = 1<<1,
        SPIE = 1<<5,
    };

    enum SIP_BITS{
        SSIP = 1<<1,
        SEIP = 1<<9,
    };

    static void write_stvec(uint64 stvec){
        asm volatile("csrw stvec, %[stvec]" : : [stvec] "r"(stvec));
    }

    static void set_sstatus(uint64 mask) {
        asm volatile("csrs sstatus, %[mask]" : : [mask] "r"(mask));
    }

    static void clear_sstatus(uint64 mask) {
        asm volatile("csrc sstatus, %[mask]" : : [mask] "r"(mask));
    }

    static void clear_sip(uint64 mask) {
        asm volatile("csrc sip, %[mask]" : : [mask] "r"(mask));
    }

    static uint64 read_scause(){
        uint64 scause;
        asm volatile("csrr %[scause], scause" : [scause] "=r"(scause));
        return scause;
    }

    static uint64 read_sepc(){
        uint64 sepc;
        asm volatile("csrr %[sepc], sepc" : [sepc] "=r"(sepc));
        return sepc;
    }

    static void write_sepc(uint64 sepc){
        asm volatile("csrw sepc, %[sepc]" : : [sepc] "r"(sepc));
    }

    static uint64 read_sstatus(){
        uint64 sstatus;
        asm volatile("csrr %[sstatus], sstatus" : [sstatus] "=r"(sstatus));
        return sstatus;
    }

    static void write_sstatus(uint64 sstatus){
        asm volatile("csrw sstatus, %[sstatus]" : : [sstatus] "r"(sstatus));
    }

    static uint64 read_a0(){
        uint64 a0;
        asm volatile("mv %[a0], a0" : [a0] "=r"(a0));
        return a0;
    }

    static void write_a0(uint64 a0){
        asm volatile("mv a0, %[a0]" : : [a0] "r"(a0));
    }

    static void write_a0_onStack(uint64 a0){
        //stari sp se nalazi u fp
        // pozicija a0 na steku a0 = x10 (offset je 10*8)
        asm volatile("sd %[a0], 80(fp)" : : [a0] "r"(a0));
    }

    static uint64 read_a1(){
        uint64 a1;
        asm volatile("mv %[a1], a1" : [a1] "=r"(a1));
        return a1;
    }

    static void write_a1(uint64 a1){
        asm volatile("mv a1, %[a1]" : : [a1] "r"(a1));
    }

    static uint64 read_a2(){
        uint64 a2;
        asm volatile("mv %[a2], a2" : [a2] "=r"(a2));
        return a2;
    }

    static void write_a2(uint64 a2){
        asm volatile("mv a2, %[a2]" : : [a2] "r"(a2));
    }

    static uint64 read_a3(){
        uint64 a3;
        asm volatile("mv %[a3], a3" : [a3] "=r"(a3));
        return a3;
    }

    static void write_a3(uint64 a3){
        asm volatile("mv a3, %[a3]" : : [a3] "r"(a3));
    }

    static uint64 read_a4(){
        uint64 a4;
        asm volatile("mv %[a4], a4" : [a4] "=r"(a4));
        return a4;
    }

    static void write_a4(uint64 a4){
        asm volatile("mv a4, %[a4]" : : [a4] "r"(a4));
    }

    static uint64 read_a5(){
        uint64 a5;
        asm volatile("mv %[a5], a5" : [a5] "=r"(a5));
        return a5;
    }

    static void write_a5(uint64 a5){
        asm volatile("mv a5, %[a5]" : : [a5] "r"(a5));
    }

    static void sretPrevMode(bool kernel);

    static void lock();

    static void unlock();

    static void supervisorTrap();

private:

    static int cnt;

    static void handleSupervisorTrap();

    static void handleConsole();

    static void handleTimer();

};

#endif //PROJEKAT_SAMOBAZA_RISCV_HPP
