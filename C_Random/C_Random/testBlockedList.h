//
//  testBlockedList.h
//  C_Random
//
//  Created by Alex Zabrodskiy on 5/28/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef testBlockedList_h
#define testBlockedList_h

#include "blockedList.hpp"

template <int P = 3, int L = 8, int THREADS = 4, int ELEMENTS = 1000>

class TestBL{
    
    static constexpr int SIZE{2*ELEMENTS*THREADS};
    
private:
    
    static void worker(BlockedList<int, P,L> & list, int* input){
        
        for(int* ptr  = input; ptr  < input + ELEMENTS*2; ++ptr)
            list.push_back(*ptr);
    
    }
    
    
    
    
public:


    
    
    static void testBL(){
        
        using namespace std;
        
        std::srand ( unsigned ( std::time(0) ) );
        
        int* inputs = new int[SIZE];
        
        for(int i = 0; i < ELEMENTS*THREADS; ++i)
            inputs[i] = i;
        
        for(int i = 0; i < ELEMENTS*THREADS; ++i)
            inputs[ELEMENTS*THREADS + i] = -i;
        
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        
        shuffle(inputs, inputs + SIZE, std::default_random_engine(seed));
        
        std::vector<std::thread> threads;
        
        BlockedList<int, P, L> list;
        
        for(int t = 0;  t < THREADS; ++t)
            threads.emplace_back(worker, ref(list), inputs + t*2*ELEMENTS);
        
        for(std::thread& thrd: threads)
            thrd.join();
        
        auto it = list.getIt(list.size());
        
        int sum{0};
        
        while(it.increment())
            sum +=  it.getNext();
        
        cout << "sum: " << sum << endl;
        
        delete[] inputs;
    
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
};












#endif /* testBlockedList_h */
