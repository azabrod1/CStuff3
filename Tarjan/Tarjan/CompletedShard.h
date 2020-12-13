//
//  CompletedShard.h
//  Tarjan4
//
//  Created by Alex Zabrodskiy on 9/2/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef CompletedShard_h
#define CompletedShard_h
//


#include "dictionary.h"


class CompletedShard{
private:
    
    std::atomic<Vid>* table;
    std::atomic<Vid> capacity;
    std::atomic<Vid> size;
    std::atomic<int> members;
    
    
public:
    
    CompletedShard(int init_pow_2 = 11) : size(1), members(0), capacity(1 << (init_pow_2)){
        table = new std::atomic<Vid>[1 << (init_pow_2)]();
    }
    
    virtual bool contains(const Vid& vertex){
        
        Vid currSize;
        do{
            
            currSize = size.load(); if(!currSize) continue; //resize in progress
            
            if(currSize*2 >= capacity){ //if load factor exceeds 0.5, resize
                if(!members && size.compare_exchange_weak(currSize, 0)){
                    if(members || size){
                        size += currSize; continue;
                    }
                    resize();
                    size = currSize;
                }
                else
                    continue;
            }
            
            members++;
            currSize = size.load();
            
            if(currSize*2 >= capacity || !currSize) --members;
            else break;
            
            
        }while(true);
        
        Vid cap(capacity), location(getHash(key, cap));
        
        while(true){
            
            Vid curr = table[location].load();
            if(curr){
                
                if(curr.key == key){ //Entry exists
                    
                    --members;
                    return true
                    
                }
                location = (location + 1) % cap; continue;
            }
            --members;
            return false;
            
            
        }
        
    }
    
    
    inline Vid getHash(const Vid key, const Vid cap){
        return key % cap;
    }
    
    void resize(){
        const Vid newCap(capacity*2); const Vid oldCap = capacity;
        std::atomic<Vid>* newTable = new std::atomic<Vid>[newCap]();
        
        Entry ent;
        
        Vid location;
        for(int e = 0; e < oldCap; ++e){
            Vid entry = table[e].load(std::memory_order_relaxed);
            if(entry.cell){
                location = getHash(entry.key, newCap);
                while(true){
                    Entry curr = newTable[location].load(std::memory_order_relaxed);
                    if(curr.cell)
                        location = (1 + location) % newCap;
                    else{
                        newTable[location].store(entry, std::memory_order_relaxed);
                        break;
                    }
                    
                }
            }
            
        }
        delete[] table;
        
        table = newTable;
        
        capacity.store(newCap);
        
    }
    
    
    
    virtual ~OpenAddressedMap(){delete[] table;}
    
    virtual std::vector<Vid>* getKeys(){return nullptr;};
    
    virtual std::vector<WeakReference<Cell<Vid>>>* getValues(){return nullptr;};
    
    
    
    virtual void deleteValues(){;}
    
    //An implementation does not need to implement this function nor does it need to guarantee thread
    //safety
    virtual unsigned long getSize() {return size.load();}
    
    
};




#endif /* CompletedShard_h */
