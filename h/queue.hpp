//
// Created by os on 5/27/23.
//

#ifndef PROJEKAT_SAMOBAZA_QUEUE_HPP
#define PROJEKAT_SAMOBAZA_QUEUE_HPP


class TCB;

class Queue {

public:

    Queue():head (nullptr), last(nullptr) {}

    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;

    virtual void put(TCB* thread);

    virtual TCB* get();

    bool empty() const {return head == nullptr;}

protected:

    TCB* head, *last;


};


#endif //PROJEKAT_SAMOBAZA_QUEUE_HPP
