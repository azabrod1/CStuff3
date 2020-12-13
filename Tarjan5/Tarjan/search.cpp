//
//  search.cpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 4/26/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#include "search.hpp"
#include "multiThreadedTarjan.hpp"
#include <algorithm>    // std::max

                              /*******************   Constructor and Destructor   ************/


Search::Search() : cellCount(0), age(0), cellBlockedOn(nullptr){ 
}

Search::~Search(){ }

void Search::setRoot(Cell<Vid>* root){
    pushToStacks(root);
}

/**
 Pre:  The cell must be owned by the thread
 Post: The cell is added to both the search's tarjan and control stack.
       Additionally, the cell is given an index and we increment cellCount
 
 @param newCell Pointer to the cell we are pushing to our stacks
 */
void Search::pushToStacks(Cell<Vid>* newCell){
    tarjanStack.push_back(newCell); controlStack.push_back(newCell);
    newCell->initIndex(cellCount++);
}

void Search::refreshCellCount(){
    cellCount = tarjanStack.back()->index + 1;
}

                        /************* Methods for node transfer ***************/

/**
 Pre:    The searches S0,S1, S2, S3,...,Sn are in a blocking cycle such that
         So --> S1 -->  S2 --> S3 -->...--> Sn
         Cells are being transferred from S0 to Sn-1 to Sn to resolve the cycle
 
 Inputs: src is one of the searches Si from S0 to Sn-1
         dest is the search Sn that is recieving the transferred cell
         conflict cell is the cell of src the search before src, S_{i-1} is blocked on and
         corresponds to the cell Ci in the paper
 
 Purpose: First, The function transfers cells from the tarjanStack of src to the tarjanStack of dest.
          Cell transfer starts at the top of src's tarjanStack and proceeds until the following two conditons are met
          a) The conflict cell is transferred to dest
          b) The last cell transferred has rank no greater than the minimum rank of the cells transferred so far
 
          ...a) is important because we want to transfer the cell causing the conflict to dest so that the conflict is 
          resolved. b) is important because we want to transfer all the cells in the same SCC as the conflict cell
          otherwise dest and src will both have some cells in that SCC on their stack, and hence they will again collide
          and form a cycle
 
          defining L to be the is the last cell we transferred from src's tarjanStack,
          we transfer all the cells in src's control stack starting from its top and 
          proceeding until we reach L. Note that L must be on src's controlStack. To see why, note that
          L.index must equal L.rank otherwise it would be impossible for L to have index lower than 
          any rank encountered so far since its own index will be less than its rank.
 
 Post:    The relevant cells discussed above are tranferred to dest with their index and rank variables updated to
          fit with their new stack. The search src is now blocked on the last cell transferred to dest
 
 */


        /*Transfer cells from source to destination */


void Search::transferCells(Search* const src, Search* const dest, Cell<Vid>* const conflictCell){

    TarjanStack &  srcTarjanStack  = src->tarjanStack;
    ControlStack& srcControlStack  = src->controlStack;
    
    //We use a reverse iterator since we are starting from the top of the stack
    auto reverseIt = srcTarjanStack.rbegin();
    
    Cell<Vid>* next = *reverseIt; Vid minRank(next->rank);
    bool reachedCC(next == conflictCell);
    

    /* Remove cells from src's tarjanStack, starting from the top and proceeding until
     a) The conflict cell is transferred to dest
     b) The latest cell transferred has rank no greater than the minimum rank of the cells transferred so far
     */
    
    while(!reachedCC || next->index > minRank){
        next = *(++reverseIt); //Increment iterator and get the next element
        minRank = std::min(minRank, next->rank);
        if(next == conflictCell) reachedCC = true;
    }
    
    //Transfer the relevent cells from src's tarjan stack to dest's tarjan stack
    
    Cell<Vid>* const last(next);
    
    Vid delta(dest->cellCount - last->index);
    
    //Carry out the transfer
    dest->tarjanStack.insert(dest->tarjanStack.end(), (reverseIt+1).base(), srcTarjanStack.end());
    
    //adjust the index and ranks and status of the transferred cells
    //to be compatible with the cell's new tarjanStack
    for(auto it = (reverseIt+1).base(); it != srcTarjanStack.end(); ++it)
        (*it)->transfer(delta, dest);
    
    //Erase the cells transferred to dest from src's tarjan stack
    //Using stl functions to accomplish this may be faster than a loop hence we use the
    //erase() function
    srcTarjanStack.erase((reverseIt+1).base(), srcTarjanStack.end());
    
    auto rit = srcControlStack.rbegin();
    
    while(*rit != last) ++rit;
    
    //Transfer cells from src control stack to destination control stack
    //starting from the controlStack's top and proceeding to the last cell transferred
    
    dest->controlStack.insert(dest->controlStack.end(), (rit+1).base(), srcControlStack.end());
    
    srcControlStack.erase((rit+1).base(), srcControlStack.end());
    
    
    //transfered to dest
    //Src now blocks on the deepest cell in the tarjan stack that we transfered to dest
    if(!srcTarjanStack.empty()){
        src->suspendOn(last);                       //TODO: relax mem order
        //last->blockSearch(src);
        last->blockSearch(src);
    }
    else
        src->removeCellBlockedOn();
    
    //CellCount of dest must be adjusted as wel added more cells 
    dest->refreshCellCount();
    
}


///** simple test function for the cell transfer method(s) */
//void Search::testCellTransfer(const Graph<Vid>& _graph, Dictionary<Vid, Cell<Vid>*>& _dict){
//    
//    MultiThreadedTarjan dummy(_graph, _dict, 1, *new LockedPendingQueue, *new UnrootedStealingQueue(0,0,_dict,1));
//    
//    
//    Search s1, s2;
//    
//    //Search 1 tarjan stack
//    //Cell<Vid> b0(0), b1(1), b2(2), l0(3), m1(4), m2(5), m3(6), m4(7), n1(8), m5(9), m6(10), t1(11);
//    
//    //Search 1 control stack b0(0), b1(1), b2(2),.... l0(3) ...., m1(4), ......n1(8), ......m5(9), t1(11)
//    
//    Cell<Vid> s1Cells[12];
//    
//    for(int c = 0; c <= 11; ++c){
//        s1Cells[c].conquer(&s1);
//        s1Cells[c].index = c;
//        s1Cells[c].rank = c;
//        s1.tarjanStack.push_back(s1Cells + c);
//    }
//    
//    for(int c : {0,1,2,3,4,8,9,10})
//        s1.controlStack.push_back(s1Cells+c);
//    
//    for(int c : {5,6,7}) s1Cells[c].rank = 4;
//    
//    s1Cells[10].rank = 3;
//    s1Cells[11].rank = 9;
//
//
//    //Search 2 tarjan stack
//    // before...  0,1,2,3,4,5,6  ....   7 ... 8,9,10,11,12,13...14...15,16,17
//    
//    //control stack    {0,5,6,7,8,13,14,15,17}
//    
//    Cell<Vid> s2Cells[18];
//    
//    for(int c = 0; c <= 17; ++c){
//        s2Cells[c].conquer(&s2);
//        s2Cells[c].index = c;
//        s2.tarjanStack.push_back(s2Cells + c);
//        s2Cells[c].rank = c;
//
//    }
//    
//    for(int c : {0,5,6,7,8,13,14,15,17})
//        s2.controlStack.push_back(s2Cells+c);
//    
//    for(int c : {0,1,2,3,4}) s2Cells[c].rank = 0;
//
//    s2Cells[15].rank = 7;
//    s2Cells[17].rank = 13;
//    
//    s1.cellCount = 12;    s2.cellCount = 18;
//
//    
//    std::cout <<"s1 stacks before\n";
//    s1.printStacks();
//    std::cout <<"s2 stacks before\n";
//    s2.printStacks();
//    
//    s1.cellBlockedOn = s2Cells + 7;
//    s2.cellBlockedOn = s1Cells + 8;
//
//    //s1.searchBlockedOn = &s2;
//    //s2.searchBlockedOn = &s1;
//    
//    std::vector<Search*> blockedSearches;
//    
//    Cell<Vid>* blockingCell = s2.cellBlockedOn;
//    
//    Search::transferCells(&s1, &s2, blockingCell, blockedSearches);
//
//    
//    
//    std::cout <<"\n**************\ns1 stacks after\n";
//    s1.printStacks();
//    std::cout <<"s2 stacks after\n";
//    s2.printStacks();
//    
//}
//
///** Test function for suspension, cycle detection and cycle resolution */
//void Search::testCellTransfer2(){
//    
//    Graph<Vid>* dummy = new DirectedHashGraph<Vid>;
//    
//    MutexDict<Vid, Cell<Vid>*> dict;
//    LockedPendingQueue q;
//    
//    MultiThreadedTarjan sample(*dummy, dict, 1, q, *new UnrootedStealingQueue(0,0, dict,1));
//    
//    //The "loop"
//    Search s1,s2,s3;
//    
//    // Auxilary Searches that will be blocked on s1..s3
//    Search s4,s5,s6,s7;
//
//    
//    //Search 1: blocked on S2 cell 7. l0 = 0. n1 = 8, suspended @ 11
//    
//    Cell<Vid> s1Cells[14], s2Cells[19], s3Cells[13];
//    
//    for(int c = 0; c < 14; ++c){
//        s1Cells[c].conquer(&s1);
//        s1Cells[c].index = c;
//        s1Cells[c].rank = c;
//        s1.tarjanStack.push_back(s1Cells + c);
//    }
//    
//    for(int c : {0,1,2,3,4,8,9,11})
//        s1.controlStack.push_back(s1Cells+c);
//    
//    s1Cells[5].rank = 0;
//    s1Cells[9].rank = 4;
//    s1Cells[13].rank = 9;
//    s1Cells[6].rank = 3;
//
//    s1.cellCount    = 14; s2.cellCount    = 19; s3.cellCount = 15;
//
//    //Search 2: blocked on s3 cell 8. l0 = 5, n1 = 7. suspended @ 15
//    
//    
//    for(int c = 0; c < 19; ++c){
//        s2Cells[c].conquer(&s2);
//        s2Cells[c].index = c;
//        s2Cells[c].rank = c;
//        s2.tarjanStack.push_back(s2Cells + c);
//    }
//    
//    for(int c : {0,5,6,7,8,13,14,15})
//        s2.controlStack.push_back(s2Cells+c);
//    
//    for(int c : {0,1,2,3,4}) s2Cells[c].rank = 0;
//    
//    s2Cells[7].rank = 5;
//    s2Cells[15].rank = 7;
//    s2Cells[12].rank = 7;
//    s2Cells[17].rank = 13;
//    
//    //Search 3: wants to suspend on s1 after cell 10. lo = 1, n1 = 8
//    
//    for(int c = 0; c < 13; ++c){
//        s3Cells[c].conquer(&s3);
//        s3Cells[c].index = c;
//        s3Cells[c].rank = c;
//        s3.tarjanStack.push_back(s3Cells + c);
//    }
//    
//    for(int c : {0,1,5,6,7,8,9,10})
//        s3.controlStack.push_back(s3Cells+c);
//    
//    s3Cells[8].rank = 5;
//    s3Cells[6].rank = 1;
//    
//    
//    std::cout <<"s1 stacks before\n";
//    s1.printStacks();
//    std::cout <<"s2 stacks before\n";
//    s2.printStacks();
//    std::cout <<"s3 stacks before\n";
//    s3.printStacks();
//    
//    assert(s1Cells[8].conquer(&s4) == CellStatus::OCCUPIED);
//    sample.suspend(&s4, s1Cells+8);
//    
//    assert(s2Cells[14].conquer(&s5) == CellStatus::OCCUPIED);
//    sample.suspend(&s5, s2Cells+14);
//    
//    
//    //First S2 suspends on S3...
//    assert(s3Cells[8].conquer(&s2) == CellStatus::OCCUPIED);
//    sample.suspend(&s2, s3Cells+8);
//    
//    //Now S1 suspends on S2
//
//    assert(s2Cells[7].conquer(&s1) == CellStatus::OCCUPIED);
//    sample.suspend(&s1, s2Cells+7);
//    
//    //Now S3 suspends on S1 to form the cycle
//
//    assert(s1Cells[8].conquer(&s3) == CellStatus::OCCUPIED);
//    sample.suspend(&s3, s1Cells+8);
//    
//    //Did the searches get properly redirected??
//    assert(s4.getBlockingSearch() == &s3);
//    //Did the search get properly redirected??
//    assert(s5.getBlockingSearch() == &s3);
//    
//    assert(s3.tarjanStackTop()->rank == s3Cells[8].index);
//    
//    std::cout <<"******************stacks after! *******************\n" <<std::endl;
//    
//    std::cout <<"s1 stacks after\n";
//    s1.printStacks();
//    std::cout <<"s2 stacks after\n";
//    s2.printStacks();
//    std::cout <<"s3 stacks after\n";
//    s3.printStacks();
//    
//    
//}
//
///** Test function for suspension, cycle detection and cycle resolution */
//void Search::testCellTransfer3(){
//    
//    Graph<Vid>* dummy = new DirectedHashGraph<Vid>;
//    
//    MutexDict<Vid, Cell<Vid>*> dict;
//    LockedPendingQueue q;
//    
//    MultiThreadedTarjan sample(*dummy, dict, 1, q, *new UnrootedStealingQueue(0,0, dict,1));
//    
//    
//    //The "loop"
//    Search s1,s2,s3;
//    
//    // Auxilary Searches that will be blocked on s1..s3
//    Search s4,s5,s6,s7;
//    
//    
//    //Search 1: blocked on S2 cell 7. l0 = 0. n1 = 8, suspended @ 11
//    s1.cellCount = 1; s3.cellCount = 18;
//    
//    
//    
//    Cell<Vid> s1Cells[1], s3Cells[19];
//    
//    
//    s1Cells[0].conquer(&s1);
//    s1Cells[0].index = 0;
//    s1Cells[0].rank = 0;
//    s1.tarjanStack.push_back(s1Cells);
//    s1.controlStack.push_back(s1Cells);
//
//
//       //Search 3: wants to suspend on s1 after cell 10. lo = 1, n1 = 8
//    
//    for(int c = 0; c < 13; ++c){
//        s3Cells[c].conquer(&s3);
//        s3Cells[c].index = c;
//        s3Cells[c].rank = c;
//        s3.tarjanStack.push_back(s3Cells + c);
//    }
//    
//    for(int c : {0,1,5,6,7,8,9,10})
//        s3.controlStack.push_back(s3Cells+c);
//    
//    s3Cells[8].rank = 5;
//    s3Cells[6].rank = 0;
//    
//    
//    std::cout <<"s1 stacks before\n";
//    s1.printStacks();
//  
//    std::cout <<"s3 stacks before\n";
//    s3.printStacks();
//    
//
//    
//    //First S3 suspends on S1...
//    assert(s1Cells[0].conquer(&s3) == CellStatus::OCCUPIED);
//    sample.suspend(&s3, s1Cells);
//    
//    //Now S1 suspends on S3
//    
//    assert(s3Cells[7].conquer(&s1) == CellStatus::OCCUPIED);
//       sample.suspend(&s1,s3Cells + 7);
//   
//    
//    //assert(s3.tarjanStackTop()->rank == s3Cells[8].index);
//    
//    std::cout <<"******************stacks after! *******************\n" <<std::endl;
//    
//    std::cout <<"s1 stacks after\n";
//    s1.printStacks();
//   
//    std::cout <<"s3 stacks after\n";
//    s3.printStacks();
//    
//    
//}


void Search::printStacks(){
    
    std::cout<< "Tarjan stack " <<std::endl;
    
    for(Cell<Vid>* cell: tarjanStack)
        std::cout << cell->index << " ";
    
    std::cout<< std::endl;
    
    std::cout<< "control stack " <<std::endl;
    
    for(Cell<Vid>* cell: controlStack)
        std::cout << cell->index << " ";
    
    std::cout << "\ncell count: " << this->cellCount;
    
    std::cout<< std::endl;
    
    
}




