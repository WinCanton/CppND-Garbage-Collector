#include "gc_pointer.h"
#include "LeakTester.h"
#include <iostream>
using namespace std;

int main()
{
    
    Pointer<int> p = new int(10);

    p = new int(21);
    p = new int(28);

    return 0;
}
