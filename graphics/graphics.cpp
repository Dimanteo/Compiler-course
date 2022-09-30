#include "graphics.h"
#include <SFML/Graphics.hpp>

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

void run(size_t width, size_t height, MandelbrotSet *mdb_set) {
    PixelArray points;
    sf::RenderWindow window(sf::VideoMode(width, height), "Mandelbrot");
    window.setFramerateLimit(60);
    drawMandelbrotSet(&points, width, height, mdb_set);

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed:
                    handleKey(event, mdb_set);
                    drawMandelbrotSet(&points, width, height, mdb_set);
                    break;
                default:
                    // Just disabling warnings
                    break;
            }
        }

        window.clear();
        window.draw(points.pixels);
        window.display();
    }
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
