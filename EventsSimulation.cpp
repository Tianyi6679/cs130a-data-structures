//
//  main.cpp
//  PA2
//
//  Created by MaTianyi on 3/9/17.
//  Copyright (c) 2017 MaTianyi. All rights reserved.
//
#include <cstdlib>
#include <cstring>
#include <random>
#include <iostream>
#include "DisjointSets.h"
#include "Graph.h"
#include "EventQ.h"
#include "fixQ.h"
#include <algorithm>
#include <vector>
#include <stdlib.h>
using namespace std;
bool sortbycost(edge x, edge y) {return x.cost<y.cost;};
void kruskal(const Graph& g, const int& numNode, int& cost,fixQ fq, bool& partitioned)
{
    cost=0;
    vector<edge> e;
    int numEdge=0;
    DisjSets ds(numNode);
    for (int i=0; i<numNode; ++i)
        for (int j=0; j<i; ++j)
            if (g.getAdjMatrix()[i][j]>0 && !fq.find(i) && !fq.find(j))
                e.push_back(edge(g.getAdjMatrix()[i][j], i, j));
                
            
    sort(e.begin(), e.end(), sortbycost);
    for (edge a : e)
    {
        if (ds.find(a.vertice1) != ds.find(a.vertice2))
        {
            cost=cost+a.cost;
	    numEdge++;
	    ds.unionSets(ds.find(a.vertice1), ds.find(a.vertice2));
	    if (numEdge==numNode-1) { partitioned =false; return;}
	}
    }
    if (numEdge==numNode-fq.size()-1) partitioned= false;
    else partitioned=true;
    return;
};
void kruskal_MST(const Graph& g, const int& numNode, int& cost, vector<edge>& MST, fixQ fq)
{
    cost=0;
    DisjSets ds(numNode);
    vector<edge> newMST;
    vector<edge> e;
    for (edge a: MST)
        if (!fq.find(a.vertice1) && !fq.find(a.vertice2))
        {
            ds.unionSets(ds.find(a.vertice1), ds.find(a.vertice2));
            cost=cost+a.cost;
            newMST.push_back(a);
        }
    for (int i=0; i<numNode; ++i)
        for (int j=0; j<i; ++j)
            if (g.getAdjMatrix()[i][j]>0 && !fq.find(i) && !fq.find(j))
                e.push_back(edge(g.getAdjMatrix()[i][j], i, j));
    sort(e.begin(), e.end(),sortbycost);
    for (edge a : e)
    {
        if (ds.find(a.vertice1) != ds.find(a.vertice2))
        {
            newMST.push_back(a);
            cost=cost+a.cost;
            ds.unionSets(ds.find(a.vertice1), ds.find(a.vertice2));
	    if (newMST.size()==numNode-1) break;
        }
    }
    
    MST.clear();
    MST=newMST;
    return;
}
bool partitioned_MST (vector<edge>& MST, fixQ fq, int numNode)
{
  //DisjSets ds(numNode);
    int numEdge=0;
    for (edge a: MST)
      if (!fq.find(a.vertice1) && !fq.find(a.vertice2)) {
	//ds.unionSets(ds.find(a.vertice1), ds.find(a.vertice2));
	numEdge++;
      }
    if (numEdge==(numNode-fq.size()-1)) return false;
    return true;
};

/*bool partitioned(const Graph& g, fixQ fq, int numNode)
{
    DisjSets ds(numNode);
    for (int i=0; i<numNode; ++i)
        for (int j=0; j<i; ++j)
            if (g.getAdjMatrix()[i][j]>0 && !fq.find(i) && !fq.find(j) && ds.find(i)!=ds.find(j))
                ds.unionSets(ds.find(i), ds.find(j));
    if (ds.numSets()-1>fq.size()) return true;
    else return false;
    }*/

int main(int argc, const char * argv[]) {
    // insert code here...
    //if (argc!=5) return -1;
    int numNode=atof(argv[3]);
    int seed=atof(argv[4]);
    int cost=0;
    int numAtker=atof(argv[1]);
    int numSysadmin=atof(argv[2]);
    int at=0;
    bool rebuild_scheduled=false;
    bool partitioned=false;
    vector<edge> MST;
    fixQ fq(numNode);
    mt19937 mt;
    mt.seed(seed);
    uniform_int_distribution<> target(0,numNode-1);
    uniform_int_distribution<> fixtime(1000,2000);
    uniform_int_distribution<> attacktime(100,1000);
    Graph g(numNode,seed);
    kruskal(g, numNode, cost, fq, partitioned);
    kruskal_MST(g, numNode, cost, MST, fq);
    EventQueue eq(numAtker);
    /* for (int i=0; i<numNode; i++){
        for (int j=0; j<numNode; j++)
            cout<<g.getAdjMatrix()[i][j]<<' ';
        cout<<endl;
	}*/
    /*for (edge i:MST) cout<<i.cost<<' '<<i.vertice1<<' '<<i.vertice2<<endl;
    cout<<MST.size()<<endl;
    cout<<partitioned<<endl;*/
    for (int i=0; i<numAtker; ++i)  eq.insert(new event(0,i,target(mt)));
    for (int i=0; i<numSysadmin; ++i) eq.insert(new event(fixtime(mt),0));
        do
    {
        if (eq.queue[1]->is_attack)
        {
            cout<<"attack("<<eq.queue[1]->time<<",attacker"<<eq.queue[1]->source<<","<<eq.queue[1]->target<<")"<<endl;
            if (!fq.find(eq.queue[1]->target)) fq.enq(eq.queue[1]->target);
            eq.insert(new event(eq.queue[1]->time+attacktime(mt),eq.queue[1]->source,target(mt)));
            at++;
            if (partitioned_MST(MST,fq,numNode) && !rebuild_scheduled)
            {
                eq.insert(new event(eq.queue[1]->time+20));
                rebuild_scheduled=true;
            }
        }
        else if (eq.queue[1]->is_fix)
        {
            if (fq.size()>0)
            {
                cout<<"fix("<<eq.queue[1]->time<<","<<fq.pop()<<")"<<endl;
                fq.deq();
            }
            eq.insert(new event(eq.queue[1]->time+fixtime(mt),0));
        }
        else if (eq.queue[1]->is_rebuild)
        {
            rebuild_scheduled=false;
            cout<<"rebuild("<<eq.queue[1]->time<<")"<<endl;
	    kruskal(g,numNode,cost,fq,partitioned);
            if (partitioned) cout<<"No possible ST"<<endl;
            else
            {
	        cout<<"Optimal ST cost is: "<<cost<<endl;
	        kruskal_MST(g, numNode, cost, MST, fq);
		cout<<"rebuilded ST has cost: "<<cost<<endl;
                cout<<"while we are missing nodes:"<<endl;
                for (int i=0; i<fq.size(); i++)
                    cout<<fq[i]<<' ';
                cout<<endl;
            }
        }
        eq.deleteMin();
    }   while (at<2000);
    return 0;
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
}
