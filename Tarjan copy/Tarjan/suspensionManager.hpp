//
//  suspensionManager2.hpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 5/19/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef suspensionManager_hpp
#define suspensionManager_hpp

#include <stdio.h>
#include <mutex>
#include "stealingQueue.hpp"
#include <vector>
#include "worker.hpp"

#define SUSPEND nullptr

#define RESUME false


class SuspensionManager{
    
    
private:
    
    /*Functions to suspend searches and retrieve which searches are suspended on which
     We abstact away the implementation of these functions from the rest of the class so that
     it is easy to change how we track suspended seaches by just altering these functions */
    
    inline void unSuspend(Search* const search){
        search->removeCellBlockedOn();
    }
    
    void runCellTransfer(const std::vector<Search*>& S, const std::vector<Cell<Vid>*>& C, const int r);
    
    
public:
    
    Search* suspend(Worker& worker, Search* const Sn, Cell<Vid>* const conflictCell);
    
    void bulkUnsuspend(BlockedList<Search*>* const suspendedList, Cell<Vid>* const prevBlockedOn, std::vector<Search*>& toResume);
 
    
};



#endif /* suspensionManager2_hpp */
