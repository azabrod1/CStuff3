//
//  blockedList.hpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 5/28/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef blockedList_hpp
#define blockedList_hpp

#include <stdio.h>
#include <atomic>

template <class S, int P = 3, int L = 8>
class BlockedListIt;


template <class S, int P = 3, int L = 8>
class BlockedList{
    
    friend class BlockedListIt<S,P,L>;
    
    
private:
    
    
public:
    
    static int sums[L];
    static int powers[L];
    
    const static int base;
    
    std::atomic<int> head;
    
    std::atomic<S*> masterList[L];
    
    BlockedList(): head(0), masterList{}{}
    
    ~BlockedList(){
        for(int arr = 0; arr < L;  ++arr)
            if(masterList[arr])
                delete[] masterList[arr].load();
    }
    
    inline int findLocation(int num){
        
        int loc{0};
        
        while(num >= sums[loc]){++loc;}
        
        return loc;
    }
    
    inline void push_back(const S& item){
        int idx(head++);
        
        if(idx < base){
            if(!*masterList){ //If we are adding the first element of the array, we must make  sure it is initilized
                
                S* toInsert(new S[base]()); S* null(nullptr);
                
                //If another thread has already added a buffer, we delete the one we created
                if(!masterList[0].compare_exchange_strong(null, toInsert))
                    delete[] toInsert;
                
            }
            
            masterList[0][idx] = item;
            
            return;
            
        }
        
        int location {findLocation(idx)};
        
        int pos(idx - sums[location-1]);
        
        
        if(!masterList[location]){
            //Create a new buffer b
            S* toInsert(new S[powers[location]]()); S* null(nullptr);
            
            //If another thread has already added a buffer, we delete the one we created
            if(!masterList[location].compare_exchange_strong(null, toInsert))
                delete[] toInsert;
        }
        
        masterList[location][pos] = item;
    }
    
    static int  initSums(){
        int _base = 1 << P;
        
        sums[0]     = _base;
        powers[0]   = _base;
        
        for(int i = 1; i < L; ++i){
            powers[i] = powers[i-1] << P;
            sums[i]   = sums[i-1] + powers[i];
        }
        
        return _base;
        
    }
    
    inline int size(){
        return head.load();
    }
    
    inline bool smallList(const int& _size){
        return _size <  base;
    }
    
    inline S& getFromSmallList(const int idx){
        return masterList[0][idx];
    }
    
    BlockedListIt<S,P,L> getIt(){
        return BlockedListIt<S,P,L>(*this, this->head);
    }
    
    BlockedListIt<S,P,L> getIt(int size){
        return BlockedListIt<S,P,L>(*this, size);
    }
    
};


template <class S, int P, int L>
struct BlockedListIt{
    
private:
    int idx, arr, ttl;
    const int last;
    BlockedList<S,P,L>& list;
    
public:
    
    BlockedListIt(BlockedList<S,P,L>& _list, int _last): idx{-1}, arr{0}, last{_last}, list(_list), ttl{-1}{}
    
    
    inline S& getNext(){
        
        return list.masterList[arr][idx];
        
    }
    
    inline bool increment(){
        
        if(++ttl == last)
            return false;
        
        if(BlockedList<S,P,L>::sums[arr] == ttl){
            ++arr; idx = 0;
        }
        else
            ++idx;
        
        
        return true;
        
        
    }
    
    
};

template<class S, int P, int L>
int BlockedList<S,P,L>::sums[L];

template<class S, int P, int L>
int BlockedList<S,P,L>::powers[L];

template<class S, int P, int L>
const int BlockedList<S,P,L>::base = BlockedList<S,P,L>::initSums();



#endif /* blockedList_hpp */
