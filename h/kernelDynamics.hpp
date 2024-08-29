//
// Created by os on 5/26/23.
//

#ifndef PROJEKAT_SAMOBAZA_KERNELDYNAMICS_HPP
#define PROJEKAT_SAMOBAZA_KERNELDYNAMICS_HPP

#include "../h/chunk.hpp"

template <typename T>
class KernelDynamics {

public:
    static KernelDynamics& create(){
        static KernelDynamics kd;
        return kd;
    }

    KernelDynamics(const KernelDynamics&) = delete;
    KernelDynamics& operator=(const KernelDynamics&) = delete;

    T* getSpace();

    void freeSpace(void *ptr);

    T* findById(uint64 id);


private:

    KernelDynamics(): head(nullptr), last(nullptr) {}

    Chunk<T> *head, *last;

};

template<typename T>
T *KernelDynamics<T>::findById(uint64 id) {
    Chunk<T> *temp = head;
    while(temp){
        T* ret = temp->findById(id);
        if(ret) return ret;
        temp = temp->next();
    }
    return nullptr;
}


template<typename T>
T *KernelDynamics<T>::getSpace() {

    Chunk<T> *temp = head;
    while(temp){
        T* ret = temp->getSpace();
        if(ret) return ret;
        temp = temp->next();
    }
    temp = Chunk<T>::create();
    if(!head)head = temp;
    else last->next() = temp;
    last = temp;

    return temp->getSpace();

}

template<typename T>
void KernelDynamics<T>::freeSpace(void *ptr) {

    Chunk<T> *temp = head;
    Chunk<T> *prev = nullptr;
    while(temp){
        int ret = temp->freeSpace((T*) ptr);
        if(!ret){
            if(temp->empty()) {
                if (prev)
                    prev->next() = temp->next();
                else
                    head = temp->next();
                if (temp == last)last = prev;
                delete temp;
            }
            break;
        }
        prev = temp;
        temp = temp->next();
    }

}

#endif //PROJEKAT_SAMOBAZA_KERNELDYNAMICS_HPP
