//
// Created by os on 4/22/23.
//

#ifndef PROJECT_BASE_MEMORYALLOCATOR_HPP
#define PROJECT_BASE_MEMORYALLOCATOR_HPP

#include "../lib/hw.h"

class MemoryAllocator {

public:

    static MemoryAllocator& create(){
        static MemoryAllocator memAll;
        return memAll;
    }

    MemoryAllocator(const MemoryAllocator&) = delete;
    MemoryAllocator& operator=(const MemoryAllocator&) = delete;

    void* mem_alloc(size_t size);

    int mem_free(void* ptr);

private:

    void editFreeSegments(void* current, void* prev, void* next, size_t size);

    void* FreeMemHead;

    MemoryAllocator(){

        FreeMemHead = const_cast<void*> (HEAP_START_ADDR);
        *((uint64*)FreeMemHead)= 0; //next freeSegment
        *((uint64*)FreeMemHead + 1) = (uint64)HEAP_END_ADDR - (uint64)HEAP_START_ADDR; //size of freeSegment

    }

};


#endif //PROJECT_BASE_MEMORYALLOCATOR_HPP
