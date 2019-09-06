// This class defines an element that is stored
// in the garbage collection information list.
//
template <class T>
class PtrDetails
{
  public:
    unsigned refcount; 
    T *memPtr;         
    bool isArray;
    unsigned arraySize; 

    PtrDetails(T *memPtr, unsigned size=0) 
    {
        // TODO: Implement PtrDetails
        this->memPtr = memPtr;
        refcount = 1;
        if (size != 0) this->isArray = true;
        else this->isArray = false;
        this->arraySize = size;
    }
};

template <class T>
bool operator==(const PtrDetails<T> &ob1,
                const PtrDetails<T> &ob2)
{
    // TODO: Implement operator==
    return (ob1.memPtr == ob2.memPtr);
}
