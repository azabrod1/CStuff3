//
//  rootedStealingQueue.hpp
//  Tarjan4
//
//  Created by Alex Zabrodskiy on 6/7/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef rootedStealingQueue_hpp
#define rootedStealingQueue_hpp

#include <stdio.h>
#include "typedefs.h"

template<class E>
class CircularVector{
    
private:
    
    const Vid logCapacity; const Vid capacity;
    E* const data;
    
    CircularVector(const Vid _logCapacity, E* const _data) : logCapacity(_logCapacity), capacity(1 << _logCapacity),  data(_data){
    }
    
    
public:
    
    CircularVector(const Vid _logCapacity) : logCapacity(_logCapacity), capacity(1 << _logCapacity), data(new E[1 <<  _logCapacity]){
    }
    
    ~CircularVector(){
        delete[] data;
    }
    
    inline E& operator[](const Vid idx){
        return  data[idx & (capacity - 1)];
    }
    
    inline E get(const Vid idx){
        data[idx & (capacity - 1)];
    }
    
    inline void put(const Vid idx, const E& element){
        data[idx & (capacity - 1)] = element;
    }
    
    inline CircularVector<E>* resize(const Vid top, const Vid bottom){
        
        E* newData = new E[capacity << 1]; //double capacity
        
        Vid mod_top(top & (capacity - 1)); Vid mod_bottom(bottom & (capacity - 1));
        
        //Data is contigious, no break
        if(mod_top < mod_bottom)
            std::copy(data + mod_top, data + mod_bottom, newData + (top & (2*capacity - 1)));
        
        //data wraps around, so handle in two copies
        else if(mod_top > mod_bottom ){
            std::copy(data + mod_top, data + capacity, newData  + (top & (2*capacity - 1)));
            //  std::copy(data, data + mod_bottom, newData + (capacity +         ));
        }
        //last two cases are for when mod_top == mod_bottom
        else if(top < bottom){
            std::copy(data +  mod_top, data + capacity, newData + mod_top);
            std::copy(data +  mod_top, data + capacity, newData + mod_top);
            
            
        }
        
        return new CircularVector<E>(1 + logCapacity,  newData);
    }
    
    void printContents(){
        for(int i = 0; i < capacity; ++i)
            std::cout << data[i] << std::endl;
    }
    
    
};




#endif /* rootedStealingQueue_hpp */
