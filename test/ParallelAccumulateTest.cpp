#include "ParallelAccumulate.h"
#include <iostream>

int main()
{
    std::vector<int> test(50);
    for (int i = 0; i < 50; i++) {
        test[i] = i;
    }
    
    std::cout << ParallelAccumulate(test.begin(), test.end(), 1) << "\n";
}