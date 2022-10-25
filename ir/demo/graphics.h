#pragma once
#include <stddef.h>

//*** Mandelbrot set properties ***//

/// Type for integer coordinates
typedef long long int coord_t;

/// Mandelbrot iterations number after which point considered to be in set 
extern const size_t ITER_MAX;

/// Precision of floating point arithmetic
extern const coord_t PRECISION;

/// Wrapper for graphics library pixel array representation
struct PixelArray;

/// Wrapper for graphic library window class
struct GraphWindow;

/// Mandelbrot set parameteres
struct MandelbrotSet {
    coord_t x_max;
    coord_t x_min;
    coord_t y_max;
    coord_t y_min;
    coord_t scale;
    coord_t x_center;
    coord_t y_center;
};

/// Routine for computing pixel colors on screen
void drawMandelbrotSet(PixelArray *points, size_t width, size_t height, MandelbrotSet *mdb_set);


//*** Graphics library interfaces ***//

/// Get drawing array
PixelArray *initPixelArray();

/// Free pixel array resources
void destrPixelArray(PixelArray *array);

/// Get graphic library window wrapper
GraphWindow *initWindow(size_t width, size_t height);

/// Free window resources
void destrWindow(GraphWindow *window);

/// Check if window opened
int isOpen(const GraphWindow *window);

/// Handle window events (e.g. key pressed).
/// Return 1 if window needs redrawing. Otherwise return 0.
void handleWindowEvents(GraphWindow *window);

/// Draw \p pixels in memory
void draw(GraphWindow *window, PixelArray *pixels);

/// Update image on the screen
void display(GraphWindow *window);

/// Set pixel color
void setPixel(PixelArray *points, size_t px, size_t py, size_t color_code);


//*** Helpers for fixed precision arithmetic ***//

// Make int from floating point number
constexpr coord_t getcoord(double x);

// Fixed precision multiply
constexpr coord_t mul(coord_t l, coord_t r);

// Fixed precision divide
constexpr coord_t div(coord_t l, coord_t r);
