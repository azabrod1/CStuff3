//
//  StealingQueue.cpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 5/7/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#include "stealingQueue.hpp"

UnrootedStealingQueue::UnrootedStealingQueue(const Vid* const toExplore, Vid size, Dictionary<Vid, Cell<Vid>*>& _dict, int _NUM_THREADS) :
vertices(toExplore), ttlCells(size), dict(_dict), NUM_THREADS(_NUM_THREADS) {
    

}

UnrootedStealingQueue::~UnrootedStealingQueue(){
    
}

WeakReference<Cell<Vid>>  UnrootedStealingQueue::next(Worker* const worker){
    Vid next(index++); Vid vertex;
    
    while(next < ttlCells){
        Cell<Vid>* const toPut(worker->spareCell);
        vertex = vertices[next];
        toPut->vertex = vertex;
        auto status = dict.put(vertex, toPut);
        
        if(status.second) //If we used up the cell, allocate a replacement
            worker->allocateSpareCell();
        
        Cell<Vid>* const retrieved(status.first);
        
        const int cellAge = retrieved->age;
        
        //Note that we check if the cell's age has changed after we read the vertex to make sure
        //that cell's age actually corresponds to the vertex
        if(retrieved->vertex == vertex && retrieved->isNew() && retrieved->age  == cellAge)
            return WeakReference<Cell<Vid>>(retrieved, cellAge);
    
        next = index++;
    }
    return nullWeakReference; //All done with cells
}
