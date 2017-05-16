//
//  fixQ.h
//  PA2
//
//  Created by MaTianyi on 3/11/17.
//  Copyright (c) 2017 MaTianyi. All rights reserved.
//

#ifndef PA2_fixQ_h
#define PA2_fixQ_h
#include<deque>
#include<iostream>
class fixQ
{
public:
    bool find(int x) {return arr[x];}
    void enq(int x) {q.push_back(x); arr[x]=true;}
    void deq() {arr[q.front()]=false; q.pop_front();}
    int operator [](int x) const {return q[x];}
    int pop();
    unsigned long size() {return q.size();}
    fixQ (int x);
private:
    bool* arr;
    std::deque<int> q;
};
fixQ::fixQ(int x)
{
    arr=new bool[x];
    for (int i=0; i<x; ++i)
        arr[i]=0;
}
int fixQ::pop()
{
    if (q.size()>0) return q.front();
    else return -1;
}
#endif
