#include <iostream>

// the mandelbrot and the colormap
#include "mandelbrot.hpp"

// Drawing mandelbrot
#define ITERATIONS 1000
#define STARTING_FRACTAL_MIN_X -2.5
#define STARTING_FRACTAL_MAX_X 1.5
#define STARTING_FRACTAL_MIN_Y -1.5
#define STARTING_FRACTAL_MAX_Y 1.5

// Screen sizes
#define SCREEN_X 1000
#define SCREEN_Y 600
#define MANDELBROT_X 1000
#define MANDELBROT_Y 800
const sf::Color background_color = sf::Color::Cyan;
const sf::Color foreground_color = sf::Color::Black;
// const sf::Color foreground_color = sf::Color(255, 154, 0);

const int screen_min_x = 0;                                        // The minimum 'x' coordinate of a visible pixel
const int screen_min_y = 1;                                        // The minimum 'y' coordinate of a visible pixel
const int screen_max_x = SCREEN_X - 1;                             // The maximum 'x' coordinate of a visible pixel
const int screen_max_y = SCREEN_Y + 1;                                 // The maximum 'y' coordinate of a visible pixel

const double fractal_min_x = STARTING_FRACTAL_MIN_X;
const double fractal_max_x = STARTING_FRACTAL_MAX_X;
const double fractal_min_y = STARTING_FRACTAL_MIN_Y;
const double fractal_max_y = STARTING_FRACTAL_MAX_Y;

// For the rendering of the Mandelbrot set
double x_pixel = (double)(fractal_max_x - fractal_min_x) / (double)(SCREEN_X - 1);
double y_pixel = (double)(fractal_max_y - fractal_min_y) / (double)(SCREEN_Y - 1);

// For panning around the Mandelbrot set
#define SENSITIVITY 10
double move_x = 0;          // We start with no movement on any of the axis
double move_y = 0;

// For complex numbers
const std::complex<double> i(0., 1.);       // The 'i' complex number

int main() {
    sf::RenderWindow window(sf::VideoMode(SCREEN_X, SCREEN_Y), "Fractal Generator");

    std::complex<double> max(STARTING_FRACTAL_MAX_X, STARTING_FRACTAL_MAX_Y);
    std::complex<double> min(STARTING_FRACTAL_MIN_X, STARTING_FRACTAL_MIN_Y);

    ColorMap cmap(background_color, foreground_color);

    Mandelbrot mandelbrot(ITERATIONS, MANDELBROT_X, MANDELBROT_Y, max, min, cmap);

    bool init = true;
    
    while(window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::Left:        // If the left arrow key is pressed
                        mandelbrot.move_fractal(-1 * SENSITIVITY, 0);
                        break;

                    case sf::Keyboard::Right:       // If the right arrow key is pressed
                        mandelbrot.move_fractal(SENSITIVITY, 0);
                        break;

                    case sf::Keyboard::Up:          // If the up arrow key is pressed
                        mandelbrot.move_fractal(0, -1 * SENSITIVITY);
                        break;

                    case sf::Keyboard::Down:        // If the down arrow key is pressed
                        mandelbrot.move_fractal(0, SENSITIVITY);
                        break;

                    case sf::Keyboard::Comma:
                        DEBUG_MSG("Zooming in.");
                        mandelbrot.zoom_fractal(SENSITIVITY / 2);
                        break;

                    case sf::Keyboard::Period:
                        DEBUG_MSG("Zooming out.");
                        mandelbrot.zoom_fractal(-1 * SENSITIVITY / 2);
                        break;

                    default:
                        break;
                }
            }
        }

        if (init) {
            window.clear(sf::Color::Black);
            window.display();
            mandelbrot.update();
            init = false;
        }

        window.clear(sf::Color::Black);
        window.draw(mandelbrot);        // Draw the Mandelbrot set itself by using the virtual void we already defined
        window.display();
    }

    return 0;
}
