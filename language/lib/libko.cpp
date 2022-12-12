#include "libko.h"
#include "graphics.h"

#include <stdio.h>

extern "C" kolang::numb_t __kolang_print(kolang::numb_t out) {
    double real_out = kolang::normalize(out);
    printf("%f\n", real_out);
    return 0;
}

extern "C" kolang::numb_t __kolang_scan() {
    double in = 0;
    scanf("%lf", &in);
    return kolang::encode(in);
}

extern "C" kolang::numb_t __kowrap_setPixel(kolang::numb_t points,
                                            kolang::numb_t px,
                                            kolang::numb_t py,
                                            kolang::numb_t color_code) {
    setPixel(reinterpret_cast<PixelArray *>(points), kolang::normalize(px),
             kolang::normalize(py), kolang::normalize(color_code));
    return 0;
}

extern "C" kolang::numb_t __kowrap_handleWindowEvents(kolang::numb_t window) {
    handleWindowEvents(reinterpret_cast<GraphWindow *>(window));
    return 0;
}

extern "C" kolang::numb_t __kowrap_isOpen(kolang::numb_t window) {
    if (isOpen(reinterpret_cast<GraphWindow *>(window))) {
        return kolang::encode(1);
    } else {
        return kolang::encode(0);
    }
}

extern "C" kolang::numb_t __kowrap_draw(kolang::numb_t window,
                                        kolang::numb_t pixels) {
    draw(reinterpret_cast<GraphWindow *>(window),
         reinterpret_cast<PixelArray *>(pixels));
    return 0;
}
extern "C" kolang::numb_t __kowrap_display(kolang::numb_t window) {
    display(reinterpret_cast<GraphWindow *>(window));
    return 0;
}

extern "C" kolang::numb_t __kowrap_destrPixelArray(kolang::numb_t array) {
    destrPixelArray(reinterpret_cast<PixelArray *>(array));
    return 0;
}

extern "C" kolang::numb_t __kowrap_destrWindow(kolang::numb_t window) {
    destrWindow(reinterpret_cast<GraphWindow *>(window));
    return 0;
}

extern "C" kolang::numb_t __kowrap_initWindow(kolang::numb_t width,
                                              kolang::numb_t height) {
    GraphWindow *winptr =
        initWindow(kolang::normalize(width), kolang::normalize(height));
    return reinterpret_cast<kolang::numb_t>(winptr);
}

extern "C" kolang::numb_t __kowrap_initPixelArray() {
    PixelArray *arrptr = initPixelArray();
    return reinterpret_cast<kolang::numb_t>(arrptr);
}