#include <SFML/Graphics.hpp>

#define ITER_MAX 500

typedef double coord_t;

struct MandelbrotSet {
    coord_t x_max;
    coord_t x_min;
    coord_t y_max;
    coord_t y_min;
    coord_t scale;
    coord_t x_center;
    coord_t y_center;
};

size_t computePointIter(coord_t re, coord_t im) {
    size_t iter_limit = ITER_MAX;
    coord_t inf_border = 2;
    // z = z^2 + c, where c = re + i * im
    coord_t z_re = re;
    coord_t z_im = im;
    for (size_t n = 0; n < iter_limit; n++) {
        coord_t sq_z_re = z_re * z_re;
        coord_t sq_z_im = z_im * z_im;
        if (sq_z_re + sq_z_re > inf_border * inf_border) {
            return n;
        }
        z_im = 2 * z_im * z_re + im;
        z_re = sq_z_re - sq_z_im + re;
    }
    return ITER_MAX;
}

sf::Color getColorForN(uint32_t n) {
    double param = (double)n / (double)ITER_MAX;
    uint8_t r = (uint8_t)(9 * (1 - param) * param * param * param * 255);
 	uint8_t g = (uint8_t)(15 * (1 - param) * (1 - param) * param * param * 255);
 	uint8_t b =  (uint8_t)(9 * (1 - param) * (1 - param) * (1 - param) * param * 255);

    return sf::Color(r, g, b);
}

void drawMandelbrotSet(sf::VertexArray &points, size_t width, size_t height, MandelbrotSet *mdb_set) {
    // Note that x_min and y_min are negative
    coord_t x_min = mdb_set->x_center + mdb_set->x_min * mdb_set->scale;
    coord_t x_max = mdb_set->x_center + mdb_set->x_max * mdb_set->scale;
    coord_t y_min = mdb_set->y_center + mdb_set->y_min * mdb_set->scale;
    coord_t y_max = mdb_set->y_center + mdb_set->y_max * mdb_set->scale;

    coord_t dx = (x_max - x_min) / (width - 1);
    coord_t dy = (y_max - y_min) / (height - 1);

    for (size_t py = 0; py < height; py++) {
        coord_t y = y_min + py * dy;
        for (size_t px = 0; px < width; px++) {
            uint32_t n = computePointIter(x_min + px * dx, y);
            points.append(sf::Vertex(sf::Vector2f(px, py), getColorForN(n)));
        }
    }
}

void handleKey(const sf::Event &event, MandelbrotSet *mdb_set) {
    switch (event.key.code) {
        case sf::Keyboard::A:
            // zoom in
            mdb_set->scale /= 1.1;
            break;
        case sf::Keyboard::S:
            // zoom out
            mdb_set->scale *= 1.1;
            break;
        case sf::Keyboard::Up:
            // move center up
            mdb_set->y_center += 0.1 * mdb_set->scale;
            break;
        case sf::Keyboard::Down:
            mdb_set->y_center -= 0.1 * mdb_set->scale;
            break;
        case sf::Keyboard::Left:
            mdb_set->x_center -= 0.1 * mdb_set->scale;
            break;
        case sf::Keyboard::Right:
            mdb_set->x_center += 0.1 * mdb_set->scale;
            break;
        default:
            break;
    }
}

int main() {
    size_t width = 600;
    size_t height = 600;

    sf::RenderWindow window(sf::VideoMode(width, height), "Mandelbrot");
    window.setFramerateLimit(60);

    sf::VertexArray points;
    MandelbrotSet mdb_set = {
        .x_max = 1.2,
        .x_min = -2.2,
        .y_max = 1.7,
        .y_min = -1.7,
        .scale = 1,
        .x_center = 0,
        .y_center = 0
    };
    drawMandelbrotSet(points, width, height, &mdb_set);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed:
                    handleKey(event, &mdb_set);
                    drawMandelbrotSet(points, width, height, &mdb_set);
                    break;
            }
        }

        window.clear();
        window.draw(points);
        window.display();
    }

    return 0;
}