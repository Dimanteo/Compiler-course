#include "libko.h"

extern "C" kolang::numb_t __kolang_print(kolang::numb_t out) {
    printf("%ld\n", out);
    return 0;
}

extern "C" kolang::numb_t __kolang_scan() {
    kolang::numb_t in = 0;
    scanf("%ld", &in);
    return in;
}