#pragma once
#include <stddef.h>

//*** Mandelbrot set properties ***//

// Type for integer coordinates
typedef long long int coord_t;

// Mandelbrot iterations number after which point considered to be in set 
extern const size_t ITER_MAX;

// Wrapper for graphics library pixel array representation
struct PixelArray;

// Mandelbrot set parameteres
struct MandelbrotSet {
    coord_t x_max;
    coord_t x_min;
    coord_t y_max;
    coord_t y_min;
    coord_t scale;
    coord_t x_center;
    coord_t y_center;
};

// Routine for computing pixel colors on screen
void drawMandelbrotSet(PixelArray *points, size_t width, size_t height, MandelbrotSet *mdb_set);


//*** Graphics library interfaces ***//

// Set pixel color
void setPixel(PixelArray *points, size_t px, size_t py, size_t color_code);

// Init window and start graphic library main loop
void run(size_t width, size_t height, MandelbrotSet *mdb_set);


//*** Helpers for fixed precision arithmetic ***//

// Make int from floating point number
coord_t inline getcoord(double x);

// Fixed precision multiply
coord_t inline mul(coord_t l, coord_t r);

// Fixed precision divide
coord_t inline div(coord_t l, coord_t r);
