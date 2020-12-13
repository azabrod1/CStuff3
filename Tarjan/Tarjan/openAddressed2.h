//
//  openAddressed2.h
//  Tarjan4
//
//  Created by Alex Zabrodskiy on 9/2/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef openAddressed2_h
#define openAddressed2_h
//
//  openAddressedSharded.h
//  Tarjan4
//
//  Created by Alex Zabrodskiy on 7/30/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//


#include "dictionary.h"

//15.7

class OpenAddressedMap2 : public Dictionary<Vid, WeakReference<Cell<Vid>>>{
    
    struct Slot{
        Slot(const Vid _key, const Vid _age, Cell<Vid>* const _cell ) noexcept : key(_key), age(_age), cell(_cell){;}
        Slot( ) noexcept : key(0), age(0), cell(0){;}
        
        std::atomic<Vid> key;
        Vid age; Cell<Vid>* cell;
    };
    
    
private:
    
    Slot* table;
    std::atomic<Vid> capacity;
    std::atomic<Vid> size;
    std::atomic<int> members;
    
    
public:
    
    OpenAddressedMap2(int init_pow_2 = 6) : size(1), members(0), capacity(1 << (init_pow_2)){
        table = new Slot[1 << (init_pow_2)]();
    }
    
    virtual std::pair<WeakReference<Cell<Vid>>,bool> put(const Vid& _key, const WeakReference<Cell<Vid>>& value){
        const Vid key = _key+1;
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
            
            Slot& curr = table[location];
            
            if(curr.key){
                
                if(curr.key == key){ //Entry exists
                    
                    if(!curr.cell) continue;

                    --members;
                    return std::make_pair<WeakReference<Cell<Vid>>,bool>(WeakReference<Cell<Vid>>(curr.cell, curr.age), false);
                    
                }
                location = (location + 1) % cap; continue;
            }
            
            
            Slot toPut(key, value.age, value.get()); Vid empty(0);
            if(table[location].key.compare_exchange_weak(empty, key)){ //successs
                table[location].age = value.age; table[location].cell = value.get();
                ++size; --members;
                return std::make_pair<WeakReference<Cell<Vid>>,bool>(WeakReference<Cell<Vid>>(value.get(), value.age), true);
            }
        }
        
    }
    
    
    inline Vid getHash(const Vid key, const Vid cap){
        return key % cap;
    }
    
    void resize(){
        const Vid newCap(capacity*2); const Vid oldCap = capacity;
        Slot* newTable = new Slot[newCap]();
        
        Slot ent;
        
        Vid location;
        for(int e = 0; e < oldCap; ++e){
            
            if(table[e].key){
                location = getHash(table[e].key, newCap);
                while(true){
                    if(newTable[location].key)
                        location = (1 + location) % newCap;
                    else{
                        newTable[location].key.store(table[e].key.load());
                        newTable[location].age = table[e].age;
                        newTable[location].cell = table[e].cell;
                        break;
                    }
    
                    
                }

                
            }
            
        }
        delete[] table;
        
        table = newTable;
        
        capacity.store(newCap);
        
    }
    
    
    
    virtual ~OpenAddressedMap2(){delete[] table;}
    
    virtual std::vector<Vid>* getKeys(){return nullptr;};
    
    virtual std::vector<WeakReference<Cell<Vid>>>* getValues(){return nullptr;};
    
    
    
    virtual void deleteValues(){;}
    
    //An implementation does not need to implement this function nor does it need to guarantee thread
    //safety
    virtual unsigned long getSize() {return size.load();}
    
    
    
};





#endif /* openAddressed2_h */
