#include <stdio.h>
#include <stdint.h>

int64_t diff_min_max(int64_t a, int64_t b);

int main() {
    printf("%ld\n", diff_min_max(2, 7));
    printf("%ld\n", diff_min_max(20, 10));
    printf("%ld\n", diff_min_max(-20, -5));
    printf("%ld\n", diff_min_max(1, 1));
    return 0;
}
