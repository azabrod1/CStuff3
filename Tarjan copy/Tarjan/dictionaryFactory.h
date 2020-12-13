//
//  DictionaryFactory.h
//  Tarjan4
//
//  Created by Alex Zabrodskiy on 7/29/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//


#ifndef DictionaryFactory_h
#define DictionaryFactory_h

#include "dictionary.h"
#include "openAddressed.h"
#include "openShardedMap.hpp"
#include "cuckooDict.h"


enum DictType {Mutex_Dict, Lock_Sharded, Sharded_SpinLock, TBB_Conc, OpenAddressed, OpenSharded, Cuckoo};

class DictionaryFactory {
    
private:
    DictionaryFactory(){;}
    
public:
    template <class K, class V>
    static Dictionary<K, V>*  getDictionary(DictType type){
        
        switch (type) {
            case Mutex_Dict:
                return new MutexDict<K, V>;
            case Lock_Sharded:
                return new ShardedMap<K, V>;
            case TBB_Conc:
                return new TBB_Concurrent_Map<K, V>;
            case Sharded_SpinLock:
                return new ShardedSpinLock<K, V>;
            case OpenAddressed:
                return new OpenAddressedMap;
            case OpenSharded:
                return new OpenAddressedShardedMap;
            case Cuckoo:
                return new CuckooMap<K, V>;
        }
    }


};


#endif /* DictionaryFactory_h */
