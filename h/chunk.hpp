//
// Created by os on 5/26/23.
//

#ifndef PROJEKAT_SAMOBAZA_CHUNK_HPP
#define PROJEKAT_SAMOBAZA_CHUNK_HPP

#include "../lib/hw.h"
#include "../h/memoryAllocator.hpp"

template <typename T>
class Chunk {

public:
    void* operator new(size_t size);
    void operator delete(void *ptr);

    Chunk(const Chunk&) = delete;
    Chunk& operator=(const Chunk&) = delete;

    ~Chunk() { }

    static Chunk* create(){
        return new Chunk();
    }

    Chunk*& next() {return  nextChunk;}

    T* getSpace();

    int freeSpace(T* ptr);

    bool empty() const {return  br == 0;}

    T* findById(uint64 id);

private:

    Chunk();

    static const uint16 CHUNK_SIZE;
    T niz[CHUNK_SIZE];
    uint16 pointers[CHUNK_SIZE];
    uint16 avail;
    uint16 br;
    Chunk* nextChunk;

};



template<typename T>
const uint16 Chunk<T>::CHUNK_SIZE = 100;

template <typename T>
void *Chunk<T>::operator new(size_t size) {
    MemoryAllocator &m = MemoryAllocator::create();
    size = (size - 1) / MEM_BLOCK_SIZE + 1;
    void *p = m.mem_alloc(size);
    return p;
}

template<typename T>
void Chunk<T>::operator delete(void *ptr) {
    MemoryAllocator &m = MemoryAllocator::create();
    m.mem_free(ptr);
}

template<typename T>
Chunk<T>::Chunk() : avail(0), br(0), nextChunk(nullptr){

    for(uint16 i=0 ; i<CHUNK_SIZE - 1 ; i++){
        pointers[i] = i+1;
    }
    pointers[CHUNK_SIZE - 1] = -1;

}

template<typename T>
T *Chunk<T>::getSpace() {
    if(avail == (uint16)(-1))
        return nullptr;
    uint16 id = avail;
    avail = pointers[avail];
    pointers[id] = -2; //TU JE ZAUZET PROSTOR, znaci nam za findById
    br++;
    return (niz + id);
}

template<typename T>
int Chunk<T>::freeSpace(T *ptr) {

    if(ptr - niz < 0 || ptr - niz >= CHUNK_SIZE)return -1;
    uint16 id = ptr - niz;
    niz[id].setUnavailable();
    pointers[id] = avail;
    avail = id;
    br--;
    return 0;

}

template<typename T>
T* Chunk<T>::findById(uint64 id) {
    uint16 cnt = 0;
    for(uint16 i = 0 ; i < CHUNK_SIZE ; i++){
        if(niz[i].getID() == id && pointers[i] == (uint16)(-2)){
            return niz + i;
        }
        else if(pointers[i] == (uint16)(-2))cnt++;
        if(br == cnt)break; //samo da malo skratimo petlju nekad
    }
    return nullptr;
}



#endif //PROJEKAT_SAMOBAZA_CHUNK_HPP
