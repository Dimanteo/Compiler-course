#ifndef KOLANG_LIBKO_H
#define KOLANG_LIBKO_H

#include "Kolang.h"

extern "C" {

using numb_t = kolang::numb_t;
extern "C" kolang::numb_t __kolang_print(kolang::numb_t num);
extern "C" kolang::numb_t __kolang_scan();
extern "C" kolang::numb_t __kowrap_setPixel(kolang::numb_t, kolang::numb_t,
                                            kolang::numb_t, kolang::numb_t);
extern "C" kolang::numb_t __kowrap_handleWindowEvents(kolang::numb_t);
extern "C" kolang::numb_t __kowrap_isOpen(kolang::numb_t);
extern "C" kolang::numb_t __kowrap_draw(kolang::numb_t, kolang::numb_t);
extern "C" kolang::numb_t __kowrap_display(kolang::numb_t);
extern "C" kolang::numb_t __kowrap_destrPixelArray(kolang::numb_t);
extern "C" kolang::numb_t __kowrap_destrWindow(kolang::numb_t);
extern "C" kolang::numb_t __kowrap_initWindow(kolang::numb_t width,
                                              kolang::numb_t height);
extern "C" kolang::numb_t __kowrap_initPixelArray();
}
#endif