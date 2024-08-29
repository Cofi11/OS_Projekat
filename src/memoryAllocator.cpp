//
// Created by os on 4/22/23.
//

#include "../h/memoryAllocator.hpp"

void *MemoryAllocator::mem_alloc(size_t size) {

    if(size <= 0)return nullptr;

    size *= MEM_BLOCK_SIZE;
    size += MEM_BLOCK_SIZE;//velicina u bajtovima, dodajemo jedan vise zbog headera gde upisujemo size, a mora da bude poravnato
    void* FreeSeg = FreeMemHead;
    void* FreeSegPrev = nullptr;

    while(FreeSeg){
        void* FreeSegNext;
        size_t FreeSegSize;

        if(*((uint64*)FreeSeg))
            FreeSegNext = (void*)(*((uint64*)FreeSeg));
        else
            FreeSegNext = nullptr;

        FreeSegSize = *((uint64*)FreeSeg + 1);

        if(FreeSegSize >= size){
            editFreeSegments(FreeSeg, FreeSegPrev, FreeSegNext, size);
            if(FreeSegSize - size < 2 * sizeof(uint64))//dodajemo taj mali odsceak alociranoj memoriji
                size = FreeSegSize;
            *((uint64*)FreeSeg) = size;
            return (void*)((uint64*)FreeSeg + MEM_BLOCK_SIZE / 8);
        }
        FreeSegPrev = FreeSeg;
        FreeSeg = FreeSegNext;
    }

    return nullptr;
}

int MemoryAllocator::mem_free(void *ptr) {
    if(!ptr) return -1;
    ptr = (uint64*)ptr - MEM_BLOCK_SIZE / 8;
    size_t size = * ((uint64*)ptr);

    void* FreeSeg = FreeMemHead;
    void* FreeSegPrev = nullptr;
    while(FreeSeg && (uint64)FreeSeg < (uint64)ptr){
        FreeSegPrev = FreeSeg;
        if(*((uint64*)FreeSeg))
            FreeSeg = (void*)(*((uint64*)FreeSeg));
        else
            FreeSeg = nullptr;
    }

    //sad treba paziti kad se sta spaja od freeSeg
    //na kraju cemo na ptr upisati FreeSeg kao next i size kao size
    if(FreeSegPrev && (uint64)FreeSegPrev + *((uint64*)FreeSegPrev + 1) == (uint64)ptr){
        if(FreeSeg && (uint64)FreeSeg == (uint64)ptr + size){
            size += *((uint64*)FreeSeg + 1);
            if(*((uint64*)FreeSeg))
                FreeSeg = (void*)(*((uint64*)FreeSeg));
            else
                FreeSeg = nullptr;
        }

        ptr = FreeSegPrev;
        size += * ((uint64*)ptr + 1);
    }
    else{
        if(FreeSegPrev){
            *((uint64*)(FreeSegPrev)) = (uint64)ptr;
        }
        else{
            FreeMemHead = ptr;
        }
        if(FreeSeg && (uint64)FreeSeg == (uint64)ptr + size){
            size += *((uint64*)FreeSeg + 1);
            if(*((uint64*)FreeSeg))
                FreeSeg = (void*)(*((uint64*)FreeSeg));
            else
                FreeSeg = nullptr;
        }
    }

    //update novog freeSeg sa next i size poljima na pocetku
    if(FreeSeg)
        *((uint64*)ptr) = (uint64)FreeSeg;
    else
        *((uint64*)ptr) = 0;
    *((uint64*)ptr + 1) = size;

    return 0;

}

void MemoryAllocator::editFreeSegments(void *current, void *prev, void *next, size_t size) {

    void* FreeNext = (void*)((uint64)current + size);
    size_t FreeNextSize = *((uint64*)current + 1) - size;
    //ako je ostatak manji od dva unit64 nece moci da se upisu polja za opis freeSeg
    if(FreeNextSize < 2* sizeof(uint64)){
        if(!prev && !next){
            FreeMemHead = nullptr; //popunjen ceo prostor heap
            return;
        }
        if(prev){
            if(next)
                *((uint64*)prev) = (uint64)next;
            else
                *((uint64*)prev) = 0;
        }
        else{
            FreeMemHead = next;
        }
        return;
    }

    if(next)
        *((uint64*)FreeNext) = (uint64)next;
    else
        *((uint64*)FreeNext) = 0;
    *((uint64*)FreeNext + 1) = FreeNextSize;

    if(prev)
        *((uint64*)prev) = (uint64)FreeNext;
    else
        FreeMemHead = FreeNext;

}
