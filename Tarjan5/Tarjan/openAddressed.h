//
//  openAddressedSharded.h
//  Tarjan4
//
//  Created by Alex Zabrodskiy on 7/30/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef openAddressedSharded_h
#define openAddressedSharded_h

#include "dictionary.h"

struct Entry{
    Entry(const Vid _key, const Vid _age, Cell<Vid>* const _cell ): key(_key), age(_age), cell(_cell){;}
    Entry( ): key(0), age(0), cell(0){;}

    const Vid key, age;
    Cell<Vid>* const cell; };

struct State{
    State(){;}

    State(Vid _size, short _members) : size(_size), members(_members){;}
     Vid size;  short members;
    inline bool operator == (const State &that) const{
        return size == that.size && members == that.members;
    }

};

class OpenAddressedMap : public Dictionary<Vid, WeakReference<Cell<Vid>>>{
private:
    
    std::atomic<Entry>* table;
    std::atomic<State> state;
    std::atomic<Vid> capacity;
    

public:
    OpenAddressedMap(int init_pow_2 = 10){
        state.store(State(0, 0));
        capacity.store( 1 << (init_pow_2));
        table = new std::atomic<Entry>[capacity];
        Entry ent;
        for(int e = 0; e < capacity; ++e)
            table[e].store(ent);
        
    }
    
    virtual std::pair<WeakReference<Cell<Vid>>,bool> put(const Vid& key, const WeakReference<Cell<Vid>>& value){
        
        State currState, nextState; Vid cap;
        do{
            currState = state.load(); cap = capacity.load();
            nextState = State(currState.size+1, currState.members+1);
                
    
        }while(currState.size*2 == cap || !state.compare_exchange_weak(currState, nextState));
        
       // std::cout << currState.size <<std::endl;
        Vid location(getHash(key, cap));
        
       // std::cout << location <<std::endl;

        while(true){
            
            Entry curr = table[location].load();
            if(curr.cell){

                if(curr.key == key){ //Entry exists

                    while(true){
                        currState = state.load();

                        if(currState.size*2 == cap && currState.members == 1){
                            resize();
                             return std::make_pair<WeakReference<Cell<Vid>>,bool>(WeakReference<Cell<Vid>>(curr.cell, curr.age), false);
                            
                        }
                        else{
                            nextState = State(currState.size, currState.members - 1);
                            if(state.compare_exchange_weak(currState, nextState))
                                return std::make_pair<WeakReference<Cell<Vid>>,bool>(WeakReference<Cell<Vid>>(curr.cell, curr.age), false);
                            
                        }
                    }
                }
                
                location = (location + 1) % cap; continue;
            }
            
            Entry toPut(key, value.age, value.get()); Entry empty;
           // std::cout << table[location].load().key <<std::endl;
             if(table[location].compare_exchange_weak(empty, toPut)){ //successs
                
                while(true){
                    currState = state.load();
                    if(currState.size *2 == cap && currState.members == 1){
                        resize();
                        return std::make_pair<WeakReference<Cell<Vid>>,bool>(WeakReference<Cell<Vid>>(value.get(), value.age), true);
                    }
                    else{
                        nextState = State(currState.size, currState.members - 1);
                        if(state.compare_exchange_weak(currState, nextState))
                            return std::make_pair<WeakReference<Cell<Vid>>,bool>(WeakReference<Cell<Vid>>(value.get(), value.age), true);
                        
                    }
                }
            }
            
        }
    
    }
    

    inline Vid getHash(const Vid key, const Vid cap){
        return key % cap;
    }
    
    
    void resize(){
        const Vid newCap(capacity*2); const Vid oldCap = capacity;
        std::atomic<Entry>* newTable = new std::atomic<Entry>[newCap]();
        Entry ent;
     
        Vid location;
        for(int e = 0; e < oldCap; ++e){
            Entry entry = table[e].load();
            if(entry.cell){
                location = getHash(entry.key, newCap);
                while(true){
                    Entry curr = newTable[location].load();
                    if(curr.cell)
                        location = (1 + location) % newCap;
                    else{
                        newTable[location].store(entry);
                        break;
                    }

                }
            }
            
        }
        delete[] table;
        
        table = newTable;
        state.store(State(state.load().size, 0));

        capacity.store(newCap);
        

    }
    
    
   // virtual V&    get(const K& key)        = 0;
    
    //Same as get
    // virtual V&    operator[](const K& key) = 0;
    
    virtual ~OpenAddressedMap(){delete[] table;}
    
    virtual std::vector<Vid>* getKeys(){return nullptr;};
    
    virtual std::vector<WeakReference<Cell<Vid>>>* getValues(){return nullptr;};
    
    //An implementation does not need to support this either
    virtual bool contains(const Vid& key){return 0;}
    
    virtual void deleteValues(){;}
    
    //An implementation does not need to implement this function nor does it need to guarantee thread
    //safety
    virtual unsigned long size() {return -1;}
    
    
};



#endif /* openAddressedSharded_h */
