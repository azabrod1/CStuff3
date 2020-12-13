//
//  singleThreadedTarjan.h
//  Tarjan
//
//  Created by Alex Zabrodskiy on 4/28/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef singleThreadedTarjan_h
#define singleThreadedTarjan_h

#include <unordered_set>
#include <unordered_map>
#include  <stack>
#include "typedefs.h"
#include <algorithm>
#include "graph.h"
#include <vector>



class Tarjan;

struct SingleCell{
    
    const static char NEW_CELL;
    const static char ON_STACK;
    const static char COMPLETE;


    SingleCell(Vid _vertex) : vertex(_vertex){;}
    
    const Vid vertex; //What vertex is the cell associated with?
    
    Vid index, rank;
    
    short status = NEW_CELL;

    std::stack<SingleCell*> unassignedNeighbors;
    
    inline void updateRank(Vid update){
        this->rank = std::min(update, this->rank);
    }
    
    inline bool complete(){
        return status == COMPLETE;
    }
    
};


class SingleThreadedTarjan{
    
    friend class Tarjan;
    
private:
    
    //Vars
    std::unordered_map<Vid, SingleCell*> lookup;
    std::stack<SingleCell*> controlStack;
    std::stack<SingleCell*> tarjanStack;
    Vid cellCount = 0;
    SCC_Set* SCCs = new SCC_Set;
    const Graph<Vid>& graph;
    
    
    //Methods
    
    SingleThreadedTarjan(const Graph<Vid>& _graph) : graph(_graph){;}
    ~SingleThreadedTarjan();

    void conquer(SingleCell* cell){
        controlStack.push(cell); tarjanStack.push(cell);
        cell->index  = cell->rank = cellCount++;
        cell->status = SingleCell::ON_STACK;
        
        for(auto& vertex: graph.getNeighborsVector(cell->vertex)){
            
            //Vertex already seen
            if(lookup.count(vertex)){
                SingleCell* neighbor = lookup[vertex];
                
                if(!neighbor->complete()) //Ignore neighbors that already belong to SCC
                    cell->unassignedNeighbors.push(neighbor);
                
            }
            
            else{
                SingleCell* neighbor = new SingleCell(vertex);
                cell->unassignedNeighbors.push(neighbor);
                lookup[vertex] = neighbor;
            }
        }
        
    }
    
    void search(SingleCell* root);
    
    SCC_Set*  run();
    
    
};


#endif /* singleThreadedTarjan_h */
