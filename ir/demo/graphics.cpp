#include "graphics.h"
#include <SFML/Graphics.hpp>

const size_t ITER_MAX = 100;
const coord_t PRECISION = 1000000;

// Make int from floating point number
constexpr coord_t getcoord(double x) {
    return x * PRECISION;
}

// Fixed precision multiply
constexpr coord_t mul(coord_t l, coord_t r) {
    return l * r / PRECISION;
}

// Fixed precision divide
constexpr coord_t div(coord_t l, coord_t r) {
    return l / r * PRECISION;
}

// Calculate color for Mandelbrot iteration number
sf::Color getColorForN(uint32_t n);

// Handle pressed key
// Keymap:
// a - zoom in
// s - zoom out
// arrows - move center
void handleKey(const sf::Event &event, MandelbrotSet *mdb_set);

struct PixelArray {
    sf::VertexArray pixels;
};

PixelArray *initPixelArray() {
    PixelArray *array = new PixelArray();
    return array;
}

void destrPixelArray(PixelArray *array) {
    delete (array);
}

struct GraphWindow {
    sf::RenderWindow window;

    GraphWindow(size_t width, size_t height) : 
        window(sf::VideoMode(width, height), "Mandelbrot") {
        window.setFramerateLimit(60);
    }
};

GraphWindow *initWindow(size_t width, size_t height) {
    GraphWindow *gw = new GraphWindow(width, height);
    return gw;
}

void destrWindow(GraphWindow *window) {
    delete(window);
}

int isOpen(const GraphWindow *window) {
    return window->window.isOpen();
}

void handleWindowEvents(GraphWindow *wrap_window) {
    sf::Event event;
    sf::Window &window = wrap_window->window;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
    }
}

void draw(GraphWindow *wrap_window, PixelArray *pixels) {
    wrap_window->window.clear();
    wrap_window->window.draw(pixels->pixels);
}

void display(GraphWindow *window) {
    window->window.display();
}

void setPixel(PixelArray *points, size_t px, size_t py, size_t color_code) {
    points->pixels.append(sf::Vertex(sf::Vector2f(px, py), getColorForN(color_code)));
}

sf::Color getColorForN(uint32_t n) {
    double param = (double)n / (double)ITER_MAX;
    uint8_t r = (uint8_t)(9 * (1 - param) * param * param * param * 255);
 	uint8_t g = (uint8_t)(15 * (1 - param) * (1 - param) * param * param * 255);
 	uint8_t b =  (uint8_t)(9 * (1 - param) * (1 - param) * (1 - param) * param * 255);

    return sf::Color(r, g, b);
}

void handleKey(const sf::Event &event, MandelbrotSet *mdb_set) {
    switch (event.key.code) {
        case sf::Keyboard::A:
            // zoom in
            mdb_set->scale = mul(mdb_set->scale, getcoord(0.9));
            break;
        case sf::Keyboard::S:
            // zoom out
            mdb_set->scale = mul(mdb_set->scale, getcoord(1.1));
            break;
        case sf::Keyboard::Up:
            // move center up
            mdb_set->y_center += mul(getcoord(0.1), mdb_set->scale);
            break;
            // move centet down
        case sf::Keyboard::Down:
            mdb_set->y_center -= mul(getcoord(0.1), mdb_set->scale);
            break;
            // move center left
        case sf::Keyboard::Left:
            mdb_set->x_center -= mul(getcoord(0.1), mdb_set->scale);
            break;
            // move center right
        case sf::Keyboard::Right:
            mdb_set->x_center += mul(getcoord(0.1), mdb_set->scale);
            break;
        default:
            // Just disabling warnings
            break;
    }
}
