#include "libko.h"

extern "C" kolang::numb_t __kolang_print(kolang::numb_t out) {
    double real_out = static_cast<double>(out) / kolang::PRECISION;
    printf("%f\n", real_out);
    return 0;
}

extern "C" kolang::numb_t __kolang_scan() {
    double in = 0;
    scanf("%lf", &in);
    return static_cast<kolang::numb_t>(in * kolang::PRECISION);
}