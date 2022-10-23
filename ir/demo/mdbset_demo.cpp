#include "mdbset_demo.h"

const size_t ITER_MAX = 100;
static const coord_t PRECISION = 1000000;

// Make int from floating point number
coord_t inline getcoord(double x) {
    return x * PRECISION;
}

// Fixed precision multiply
coord_t inline mul(coord_t l, coord_t r) {
    return l * r / PRECISION;
}

// Fixed precision divide
coord_t inline div(coord_t l, coord_t r) {
    return l / r * PRECISION;
}

size_t computePointIter(coord_t re, coord_t im) {
    size_t iter_limit = ITER_MAX;
    coord_t inf_border = 2;
    // z = z^2 + c, where c = re + j * im
    coord_t z_re = re;
    coord_t z_im = im;
    for (size_t n = 0; n < iter_limit; n++) {
        coord_t sq_z_re = mul(z_re, z_re);
        coord_t sq_z_im = mul(z_im, z_im) ;
        if (sq_z_re + sq_z_re > inf_border * inf_border * PRECISION) {
            return n;
        }
        z_im = 2 * mul(z_im, z_re) + im;
        z_re = sq_z_re - sq_z_im + re;
    }
    return ITER_MAX;
}

void drawMandelbrotSet(PixelArray *points, size_t width, size_t height, MandelbrotSet *mdb_set) {
    // Note that x_min and y_min are negative
    coord_t x_min = mdb_set->x_center + mul(mdb_set->x_min, mdb_set->scale);
    coord_t x_max = mdb_set->x_center + mul(mdb_set->x_max, mdb_set->scale);
    coord_t y_min = mdb_set->y_center + mul(mdb_set->y_min, mdb_set->scale);
    coord_t y_max = mdb_set->y_center + mul(mdb_set->y_max, mdb_set->scale);

    coord_t dx = (x_max - x_min) / (width - 1);
    coord_t dy = (y_max - y_min) / (height - 1);

    for (size_t py = 0; py < height; py++) {
        coord_t y = y_min + py * dy;
        for (size_t px = 0; px < width; px++) {
            size_t n = computePointIter(x_min + px * dx, y);
            setPixel(points, px, py, n);
        }
    }
}

void zoom(MandelbrotSet *mdb_set) {
    mdb_set->scale = mul(mdb_set->scale, getcoord(0.8));;
}

void startMdbsetDemo() {
    size_t width = 600;
    size_t height = 600;

    // Setup Mandelbrot set
    MandelbrotSet mdb_set = {
        .x_max = getcoord(1.2),
        .x_min = getcoord(-2.2),
        .y_max = getcoord(1.7),
        .y_min = getcoord(-1.7),
        .scale = getcoord(1),
        .x_center = getcoord(-0.7436438870),
        .y_center = getcoord(0.13182590)
    };

    PixelArray *points = initPixelArray();
    GraphWindow *window = initWindow(width, height);
    size_t iterations = 0;
    size_t step_number = 35;

    // Main loop
    while (step_number && isOpen(window)) {
        // Handle pressed keys
        handleWindowEvents(window);

        // Animate
        if (iterations == 0) {
            zoom(&mdb_set);
            drawMandelbrotSet(points, width, height, &mdb_set);
            step_number--;
        }

        draw(window, points);
        display(window);

        iterations = (iterations + 1) % 2;
    }

    destrPixelArray(points);
    destrWindow(window);
}