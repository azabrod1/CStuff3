//
//  WeakReference.h
//  Tarjan4
//
//  Created by Alex Zabrodskiy on 6/5/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef WeakReference_h
#define WeakReference_h

#include "typedefs.h"
#include "Reference.hpp"


/** The weak reference struct stores a pointer to a recyclable object and its 
 age variable so that the user knows when the reference has expired
 
 
 */
template <class E>
struct WeakReference{
    
    E*  subject;
    Vid age;
    
    WeakReference() : age(0){}
    
    WeakReference(E* const  _subject, const Vid _age) : subject(_subject), age(_age){}
    
    WeakReference( WeakReference<E> && _ref): subject(_ref.subject), age(_ref.age){ }
    WeakReference(const WeakReference<E> & _ref): subject(_ref.subject), age(_ref.age){ }
    
    WeakReference<E>& operator = ( const WeakReference<E> & ref) = default;
    WeakReference<E>& operator = ( WeakReference<E> && ref)      = default;
    
    
    inline bool operator == (WeakReference<E> that) {
        return (that.subject == this->subject) && (that.age == this->age);
    }
    
    inline bool operator != (WeakReference<E> that){
        return (that.subject != this->subject) || (that.age != this->age);
    }
    
    inline E* get() const{
        return subject;
    }
    
    
    //Is the reference expired? In other words, is the object's age not equal to the age when the reference was made
    inline bool isExpired(){
        return subject->age != age;
    }
        
};

extern WeakReference<Cell<Vid>> nullWeakReference;

#endif /* WeakReference_h */
