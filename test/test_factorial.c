#include <stdio.h>
#include <stdint.h>

int64_t factorial(int64_t n); 

int main() {
    printf("%ld\n", factorial(0));
    printf("%ld\n", factorial(5));
    printf("%ld\n", factorial(10));
    //printf("%ld\n", factorial(-1));
    return 0;
}
