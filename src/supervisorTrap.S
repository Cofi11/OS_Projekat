.align 4
.extern _ZN5Riscv20handleSupervisorTrapEv
.extern _ZN5Riscv13handleConsoleEv
.extern _ZN5Riscv11handleTimerEv
.extern _ZN3TCB7runningE

.global _ZN5Riscv14supervisorTrapEv
.type _ZN5Riscv14supervisorTrapEv @function
_ZN5Riscv14supervisorTrapEv:
    j ec_handler
    j tim_handler
    .skip 28
    j ex_handler


ec_handler:
    //prelazak na sistemski stek, ne treba kad sam u kernelu!!!!
    csrw sscratch, a0

    ld a0, _ZN3TCB7runningE
    sd sp, 2 * 8(a0) //cuvamo sp u TCB::sp
    ld sp, 3 * 8(a0) //upisujemo u sp, iz TCB::ssp
    csrr a0, sscratch

    addi sp,sp,-256

    .irp ind,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\ind, \ind * 8(sp)
    .endr

    call _ZN5Riscv20handleSupervisorTrapEv

    .irp ind,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\ind, \ind * 8(sp)
    .endr

    addi sp,sp,256

    //vracanje na korisnicki stek
    csrw sscratch, a0

    ld a0, _ZN3TCB7runningE
    sd sp, 3 * 8(a0) //cuvamo ssp u TCB::ssp
    ld sp, 2 * 8(a0) //upisujemo sp iz TCB::sp
    csrr a0, sscratch

    sret


tim_handler:
    //prelazak na sistemski stek, ne treba kad sam u kernelu!!!!
    csrw sscratch, a0

    ld a0, _ZN3TCB7runningE
    sd sp, 2 * 8(a0) //cuvamo sp u TCB::sp
    ld sp, 3 * 8(a0) //upisujemo u sp, iz TCB::ssp
    csrr a0, sscratch

    addi sp,sp,-256

    .irp ind,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\ind, \ind * 8(sp)
    .endr

    call _ZN5Riscv11handleTimerEv

    .irp ind,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\ind, \ind * 8(sp)
    .endr

    addi sp,sp,256

    //vracanje na korisnicki stek
    csrw sscratch, a0

    ld a0, _ZN3TCB7runningE
    sd sp, 3 * 8(a0) //cuvamo ssp u TCB::ssp
    ld sp, 2 * 8(a0) //upisujemo sp iz TCB::sp
    csrr a0, sscratch

    sret


ex_handler:
    //prelazak na sistemski stek, ne treba kad sam u kernelu!!!!
    csrw sscratch, a0

    ld a0, _ZN3TCB7runningE
    sd sp, 2 * 8(a0) //cuvamo sp u TCB::sp
    ld sp, 3 * 8(a0) //upisujemo u sp, iz TCB::ssp
    csrr a0, sscratch

    addi sp,sp,-256

    .irp ind,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\ind, \ind * 8(sp)
    .endr

    call _ZN5Riscv13handleConsoleEv

    .irp ind,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\ind, \ind * 8(sp)
    .endr

    addi sp,sp,256

    //vracanje na korisnicki stek
    csrw sscratch, a0

    ld a0, _ZN3TCB7runningE
    sd sp, 3 * 8(a0) //cuvamo ssp u TCB::ssp
    ld sp, 2 * 8(a0) //upisujemo sp iz TCB::sp
    csrr a0, sscratch

    sret