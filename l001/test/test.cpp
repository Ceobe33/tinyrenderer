#include <iostream>
#include <stdio.h>

int main(int argc, char* argv[]) {
    unsigned int* var1 = new unsigned int;
    unsigned int var2 = 123;
    delete var1;
    if (var1 != NULL) {
        var2 = *(unsigned int*)var1;
    }

    size_t size = static_cast<size_t>(-1);
    char* ptr = static_cast<char*>(malloc(size));
    std::cout << size << std::endl;

    // q: what is the value of size?
    // A. -1    B. 4294967295   C. no right answer

    return 0;
}
