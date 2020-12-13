//
//  multiThreadedTarjan.cpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 4/30/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#include "multiThreadedTarjan.hpp"
#include "tarjan.hpp"


MultiThreadedTarjan::~MultiThreadedTarjan(){
   
}


SCC_Set* MultiThreadedTarjan::run()
{
    //Cell<Vid>::ALGO_IN_PROGRESS = true;

    SCC_Set* SCCs = new SCC_Set;
    
    std::vector<Worker> workers;
    std::vector<std::thread> threads;
    
    for(int ID = 0; ID < NUM_THREADS; ++ID)
        workers.emplace_back(ID, *this, graph, dict);
    
    for(Worker& worker: workers)
        threads.emplace_back(std::ref(worker));
    
    
    for(std::thread& thrd: threads )
        thrd.join();
    
        
    for(Worker& worker: workers)
        SCCs->insert(worker.SCCs.begin(), worker.SCCs.end());
    
    std::cout << "s2:  " <<  s2 <<std::endl;
    
    s2 =  0;
 
    return SCCs;
}



Search* MultiThreadedTarjan::getSearch(Worker* worker){
    Search* search;
    flags.store(0); //Can prob use relaxed mem order here
    bool updateFlagsASAP(true); //Should we update flags next loop iteration
    bool done(false);
    const long mask(worker->MASK);
    
    while(!done){

        //First, see if there is a pending Search we can resume
        search = pending.get();
        
        if(search) return search;
        
        //Otherwise, try to start new search by taking a cell off the Stealing queue
        WeakReference<Cell<Vid>> root(cellQueue.next(worker));
        
        /* We try to find a potential root cell on the stealing queue.
         * we need to try to conquer it before starting the search.
         * Otherwise, we could be starting a search on a cell already
         * in another search's stack! If the conquer suceeds, we can start the search
         * otherwise we continue this loop
         */
        
        
        if(root.get())
            if(root.get()->claimOrFail(worker->spareSearch)){ //Found a viable root for the new search
                worker->initNeighbors(root.get());
                search = worker->spareSearch;
                search->setRoot(root.get());
                worker->allocateSpareSearch();
                return search;
            }
            
         
        
        if(updateFlagsASAP)
            flags|=mask;
        
        //Update flags next iteration if they were not updated this time and our flag is not set
        updateFlagsASAP = (!updateFlagsASAP && ((flags.load() & mask) == 0));
        
        done = (flags.load() == ALL_FLAGS_SET);
        
        if(!done){
            std::this_thread::sleep_for(std::chrono::milliseconds(1));

        }
        
    }
    
    return nullptr;
    
}
