//
//  EventQ.h
//  PA2
//
//  Created by MaTianyi on 3/11/17.
//  Copyright (c) 2017 MaTianyi. All rights reserved.
//

#ifndef PA2_EventQ_h
#define PA2_EventQ_h
#include <iostream>
#include <math.h>
class event
{
public:
    double time;
    const double epsilon=0.001;
    event(double t, int sr, int tg ): time(t), source(sr), target(tg), is_attack(1),is_fix(0),is_rebuild(0) {};
    event(double t, int tg): time(t), target(tg), is_fix(1), is_attack(0), is_rebuild(0) {};
    event(double t): time(t), is_rebuild(1), is_fix(0), is_attack(0) {};
    bool is_attack,is_fix,is_rebuild;
    int source, target;
    bool operator <(event& rhs)
    {
        
        if (fabs(this->time-rhs.time)<epsilon)
        {
            if (this->is_fix & !rhs.is_fix) return 1;
            else if (this->is_rebuild) return rhs.is_attack;
            else return 0;
        }
        else return this->time<rhs.time;
    };
    bool operator <=(event& rhs)
    {
        
        if (fabs(this->time-rhs.time)<epsilon)
        {
            if (this->is_fix & !rhs.is_fix) return 1;
            else if (this->is_rebuild) return !rhs.is_fix;
            else return rhs.is_attack;
        }
        return this->time<rhs.time;
    }
};

class EventQueue
{
public:
    EventQueue( unsigned int alloc=100)
    {
        capacity=alloc;
        size=0;
        queue = new event*[capacity];
    };
    ~EventQueue()
    {
        for (unsigned int i=1; i<=size; ++i) delete queue[i];
        delete [] queue;
    }
    bool isEmpty() const {return size==0;}
    void insert(event* x)
    {
        if (size == capacity - 1)
            resize(capacity * 2);
        //percolate up
        unsigned int hole = ++size;
        event* copy=x;
        queue[0]=copy;
        for(; *x < *(queue[hole/2]); hole /=2)
            queue[hole]=queue[hole/2];
        queue[hole]=queue[0];
    }
    void deleteMin ()
    {
        if (isEmpty()) return;
        delete queue[1];
        queue[1]=queue[size--];
        percolateDown(1);
    }
    unsigned int get_size(){return size;};
    unsigned int size;
    unsigned int capacity;
    event **queue;
    void resize(unsigned int new_capacity)
    {
        if (new_capacity<=capacity) return;
        event** new_queue;
        new_queue=new event*[new_capacity];
        for (unsigned i=1; i<=size; i++)
            new_queue[i]=queue[i];
        
        delete [] queue;
        queue=new_queue;
        capacity=new_capacity;
    };
    void percolateDown( unsigned int hole)
    {
        unsigned int child;
        event* tmp = queue[hole];
        for (; hole*2<=size; hole=child)
        {
            child=hole*2;
            if (child!=size && *(queue[child+1])<*(queue[child]))
                ++child;
            if (*(queue[child])<= *(tmp) ) queue[hole]=queue[child];
            else
                break;
        }
        queue[hole]=tmp;
    };
    void decreaseKey(unsigned int p, double delta)
    {
        if (delta<=0) return;
        queue[p]->time =queue[p]->time-delta;
        //percolate up
        unsigned int hole=p;
        event* copy=queue[p];
        queue[0]=copy;
        for(; *copy < *(queue[hole/2]); hole /=2)
            queue[hole]=queue[hole/2];
        queue[hole]=queue[0];
    };
    void increaseKey(unsigned int p, double delta)
    {
        
        if (delta<=0) return;
        queue[p]->time =queue[p]->time+delta;
        percolateDown(p);
    }
};


#endif
