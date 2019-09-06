#include <iostream>
#include <list>
#include <typeinfo>
#include <cstdlib>
#include "gc_details.h"
#include "gc_iterator.h"

template <class T, int size = 0>
class Pointer{

private:
    static std::list<PtrDetails<T> > refContainer;
    T* addr;
    bool isArray; 
    unsigned arraySize;
    static bool first; 
    // Return an iterator to pointer details in refContainer.
    typename std::list<PtrDetails<T> >::iterator findPtrInfo(T *ptr);

public:
    // Define an iterator type for Pointer<T>.
    typedef Iter<T> GCiterator;

    Pointer(){
        Pointer(NULL);
    }

    Pointer(T*);

    Pointer(const Pointer &);

    ~Pointer();

    // Collect garbage. Returns true if at least
    // one object was freed.
    static bool collect();

    // Overload assignment of pointer to Pointer.
    T *operator=(T *t);

    // Overload assignment of Pointer to Pointer.
    Pointer &operator=(Pointer &rv);

    // Return a reference to the object pointed
    // to by this Pointer.
    T &operator*(){
        return *addr;
    }

    // Return the address being pointed to.
    T *operator->() { return addr; }

    // Return a reference to the object at the
    // index specified by i.
    T &operator[](int i){ return addr[i];}
    
    // Conversion function to T *.
    operator T* () { return addr; }

    // Return an Iter to the start of the allocated memory.
    Iter<T> begin(){
        int _size;
        if (isArray)
            _size = arraySize;
        else
            _size = 1;
        return Iter<T>(addr, addr, addr + _size);
    }


    // Return an Iter to one past the end of an allocated array.
    Iter<T> end(){
        int _size;
        if (isArray)
            _size = arraySize;
        else
            _size = 1;
        return Iter<T>(addr + _size, addr, addr + _size);
    }


    // Return the size of refContainer for this type of Pointer.
    static int refContainerSize() { return refContainer.size(); }

    // A utility function that displays refContainer.
    static void showlist();

    // Clear refContainer when program exits.
    static void shutdown();

};


// STATIC INITIALIZATION
// Creates storage for the static variables
template <class T, int size>
std::list<PtrDetails<T> > Pointer<T, size>::refContainer;


template <class T, int size>
bool Pointer<T, size>::first = true;


// Constructor for both initialized and uninitialized objects. -> see class interface
template<class T,int size>
Pointer<T,size>::Pointer(T *t){
    // Register shutdown() as an exit function.
    if (first)
        atexit(shutdown);
    first = false;

    // TODO: Implement Pointer constructor
    // Initialise Pointer class variables
    addr = t;
    if (size == 0) {
        isArray = false;
        arraySize = size;
    }
    else {
        isArray = true;
        arraySize = size;
    }

    // Create PtrDetails entry for the object in refContainer
    PtrDetails<T> p_d(t, size);
    refContainer.push_back(p_d);
}


// Copy constructor.
template< class T, int size>
Pointer<T,size>::Pointer(const Pointer &ob){

    // TODO: Implement Pointer constructor
    // Initialise new Pointer variables
    addr = ob.addr;
    arraySize = ob.arraySize;
    isArray = ob.isArray;
    
    // Increase refcount for corresponding PtrDetails entry in refCount
    typename std::list<PtrDetails<T>>::iterator p;
    p = findPtrInfo(ob.addr);
    p->refcount++;
}


// Destructor for Pointer.
template <class T, int size>
Pointer<T, size>::~Pointer(){
    
    // TODO: Implement Pointer destructor
    typename std::list<PtrDetails<T>>::iterator p;
    p = findPtrInfo(addr);

    if (p->refcount)
        p->refcount--;
    
    collect();
}


// Collect garbage. Returns true if at least
// one object was freed.
template <class T, int size>
bool Pointer<T, size>::collect(){

// TODO: Implement collect function
    bool memfreed = false;
    
    typename std::list<PtrDetails<T>>::iterator p;
    do {
        for (p = refContainer.begin(); p != refContainer.end(); p++) {
            if (p->refcount > 0)
                continue;
            memfreed = true;
            refContainer.remove(*p);
            
            if (p->memPtr) {
                if (p->isArray) delete[] p->memPtr;
                else delete p->memPtr;
            }
            break;
        } 
    } while (p != refContainer.end());
    return memfreed;
}


// Overload assignment of pointer to Pointer.
template <class T, int size>
T *Pointer<T, size>::operator=(T *t){

    // TODO: Implement operator==
    // Find corresponding PtrDetails entry in refContainer and reduce refcount

    typename std::list<PtrDetails<T>>::iterator p;
    p = findPtrInfo(addr);
    p->refcount--;
    p = findPtrInfo(t);
    
    // Create new PtrDetails entry in refContainer
    PtrDetails<T> p_d(t, size);
    refContainer.push_back(p_d);

    // Re-initialise Pointer variables
    addr = p_d.memPtr;
    isArray = p_d.isArray;
    arraySize = p_d.arraySize;
    return addr;
}


// Overload assignment of Pointer to Pointer.
template <class T, int size>
Pointer<T, size> &Pointer<T, size>::operator=(Pointer &rv){

    // TODO: Implement operator==
    // Decrement refcount for current object
    typename std::list<PtrDetails<T>>::iterator p;
    p = findPtrInfo(addr);
    p->refcount--;

    // Increment refcount for object pointed to by &rv
    p = findPtrInfo(rv.addr);
    p->refcount++;
    addr = rv.addr;
    return rv;
}

// A utility function that displays refContainer.
template <class T, int size>
void Pointer<T, size>::showlist(){
    typename std::list<PtrDetails<T> >::iterator p;
    std::cout << "refContainer<" << typeid(T).name() << ", " << size << ">:\n";
    std::cout << "memPtr refcount value\n ";
    if (refContainer.begin() == refContainer.end())
    {
        std::cout << " Container is empty!\n\n ";
    }
    for (p = refContainer.begin(); p != refContainer.end(); p++)
    {
        std::cout << "[" << (void *)p->memPtr << "]"
             << " " << p->refcount << " ";
        if (p->memPtr)
            std::cout << " " << *p->memPtr;
        else
            std::cout << "---";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}


// Find a pointer in refContainer.
template <class T, int size>
typename std::list<PtrDetails<T> >::iterator
Pointer<T, size>::findPtrInfo(T *ptr){
    typename std::list<PtrDetails<T> >::iterator p;
    // Find ptr in refContainer.
    for (p = refContainer.begin(); p != refContainer.end(); p++)
        if (p->memPtr == ptr)
            return p;
    return p;
}


// Clear refContainer when program exits.
template <class T, int size>
void Pointer<T, size>::shutdown(){
    if (refContainerSize() == 0)
        return; // list is empty
    typename std::list<PtrDetails<T> >::iterator p;
    for (p = refContainer.begin(); p != refContainer.end(); p++)
    {
        // Set all reference counts to zero
        p->refcount = 0;
    }
    collect();
}
