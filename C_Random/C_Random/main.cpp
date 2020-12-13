//
//  main.cpp
//  C_Random
//
//  Created by Alex Zabrodskiy on 4/19/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#include <iostream>
#include <thread>
#include <vector>
#include <unordered_map>
#include <atomic>
#include <time.h>
#include <memory>
#include <random>
#include "Aqueue.h"
#include "blockedList.cpp"
#include "testBlockedList.h"

#include "tbb/critical_section.h"
#include "tbb/concurrent_unordered_map.h"
#include <tbb/concurrent_queue.h>

using namespace std;

const static int N = 150;

mutex mu;

void idk();

std::atomic<int> accum(0);

struct A {
    int a[100];
    
};

struct B : enable_shared_from_this<B>{
    weak_ptr<int> ptr;
    int useless;
    short balls;
    //pointer to object, some refernece count, pointer to strong
};


tbb::concurrent_unordered_map<int, int> map(100);

int M = 100;

void func(int base){
    
    for(int i = 0; i < M; ++i)
        map[i] = i+1000;
    
}


typedef tbb::tbb_allocator<std::pair<const int, int>> alloc;

int main(int argc, const char * argv[]) {
    int * c = new int[100]; delete[] c;
    
    idk();

}


struct Node{
    
    int* cell;
    int vertex;
    
    Node(){};
    
    Node(const int _vertex) : vertex(_vertex){;}
    
    ~Node(){
        cout << "deleted" << endl;
    }
    
};

class NodeDeleter{
public:
    void operator()(Node* const n){
        cout << n->vertex << endl;
    }
};

void idk(){
    
    Node* node = new Node(5);
    
   // shared_ptr<Node> ptr(node);
    
   // cout << node << endl;
    
    shared_ptr<Node> ptr2(node , NodeDeleter());
    
    cout << node->vertex <<endl;

}












