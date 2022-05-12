#include <iostream>
#include <SFML/Graphics.hpp>

// Debugging
#define DEBUG 1
#ifdef DEBUG
#define DEBUG_MSG(str) do { std::cout << str << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif

// Screen sizes
#define SCREEN_X 800
#define SCREEN_Y 600
const int min_x = 0;                                        // The minimum 'x' coordinate of a visible pixel
const int min_y = 1;                                        // The minimum 'y' coordinate of a visible pixel
const int max_x = SCREEN_X - 1;                             // The maximum 'x' coordinate of a visible pixel
const int max_y = SCREEN_Y;                                 // The maximum 'y' coordinate of a visible pixel

// Drawing mandelbrot
#define ITERATIONS 1000

sf::VertexArray screen(sf::Points, SCREEN_X * SCREEN_Y);

int get_screen_pos(const int& x, const int& y) {
    return x + max_x * (y - 1);
}

/**
 * Initialize the pixel's positions and colors. 
 * Only done at the start.
 */
static void init_pixels() {
    for (int x = min_x; x <= max_x; ++x) {                                      // For every 'x' coordinate
        for (int y = min_y; y <= max_y; ++y) {                                  // For every 'y' coordinate 
            int screen_pos = get_screen_pos(x, y);
            screen[screen_pos].position = sf::Vector2f(x, y);          // We set the corresponding pixel's position
            screen[screen_pos].color = sf::Color::Black;               // And we make it black
        }
    }

    DEBUG_MSG("Pixel initialization completed.");
}

static sf::Color calculate_point(const int& x, const int& y) {
    int z_x, z_y;                                               // Create a complex number that corresponds to the screen's coordinates
    z_x = x;
    z_y = y;
    int i;
    for (i = 0; i < ITERATIONS; ++i) {
        // We now have to square 'z'
        z_x = z_x * z_x - z_y * z_y;
        z_y = 2 * z_x * z_y;

        // And add the original parameters
        z_x += x;
        z_y += y;

        // We check if the length of the vector/complex number is greater than 2
        // Note that we square the length, in order to improve performance (square root is a slow computation)
        if (z_x * z_x + z_y * z_y > 4) {
            break;
        }
    }

    if (i == ITERATIONS) {
        return sf::Color::White;
    }
    return sf::Color::Black;
}

/**
 * Render the Mandelbrot set for a fixed number of iterations.
 * @param num_iterations : the number of iterations for which we are rendering the set
 */
static void render_mandelbrot() {
    for (int x = min_x; x < max_x; ++x) {
        for (int y = min_y; y < max_y; ++y) {
            sf::Color current_pixel_color = calculate_point(x, y);
            screen[get_screen_pos(x, y)].color = current_pixel_color;
        }
    }

    DEBUG_MSG("Mandelbrot set rendered.");
}

int main() {
    sf::RenderWindow window(sf::VideoMode(SCREEN_X, SCREEN_Y), "Fractal Generator");

    init_pixels();

    render_mandelbrot();
    
    while(window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(screen);
        window.display();
    }

    return 0;
}
