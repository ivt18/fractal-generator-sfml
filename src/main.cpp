#include <iostream>
#include <SFML/Graphics.hpp>

// Debugging
#define DEBUG 1
#ifdef DEBUG
#define DEBUG_MSG(str) do { std::cout << str << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif

// Drawing mandelbrot
#define ITERATIONS 1000
#define STARTING_FRACTAL_MIN_X -2.5
#define STARTING_FRACTAL_MAX_X 1.5
#define STARTING_FRACTAL_MIN_Y -1.5
#define STARTING_FRACTAL_MAX_Y 1.5

// Screen sizes
#define SCREEN_X 800
#define SCREEN_Y 600
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

sf::VertexArray screen(sf::Points, SCREEN_X * SCREEN_Y);

int get_screen_pos(const int& x, const int& y) {
    return x + screen_max_x * (y - 1);
}

/**
 * Initialize the pixel's positions and colors. 
 * Only done at the start.
 */
static void init_pixels() {
    for (int x = screen_min_x; x <= screen_max_x; ++x) {                                      // For every 'x' coordinate
        for (int y = screen_min_y; y <= screen_max_y; ++y) {                                  // For every 'y' coordinate 
            int screen_pos = get_screen_pos(x, y);
            screen[screen_pos].position = sf::Vector2f(x, y);          // We set the corresponding pixel's position
            // screen[screen_pos].color = sf::Color::Black;               // And we make it black
        }
    }

    DEBUG_MSG("Pixel initialization completed.");
}

static sf::Color calculate_point(const int& x, const int& y) {
    double z_x, z_y, c_x, c_y;              // Create a complex number that corresponds to the screen's coordinates
    z_x = fractal_min_x + x * x_pixel;
    z_y = fractal_max_y - y * y_pixel;

    c_x = z_x;                              // Create the constant that we add with each iteration
    c_y = z_y;
    int i;
    for (i = 0; i < ITERATIONS; ++i) {
        // We now have to square 'z'
        // And add the original parameters
        double tmp = z_x;           // Store temporarily old value of 'z_x'. Used for computing 'z_y'
        z_x = z_x * z_x - z_y * z_y + c_x;
        z_y = 2 * tmp * z_y + c_y;

        // We check if the length of the vector/complex number is greater than 2
        // Note that we square the length, in order to improve performance (square root is a slow computation)
        if (z_x * z_x + z_y * z_y > 4) {
            break;          // If it diverges, we then end the for-loop
        }
    }

    if (i == ITERATIONS) {
        return sf::Color::White;
    }
    return sf::Color::Black;
}

/**
 * Render the bottom left quartile of the Mandelbrot set.
 */
void render_bottom_left() {
    for (int x = screen_min_x; x < screen_max_x / 2; ++x) {
        for (int y = screen_max_y / 2; y < screen_max_y; ++y) {
            sf::Color current_pixel_color = calculate_point(x, y);
            screen[get_screen_pos(x, y)].color = current_pixel_color;
        }
    }
}

/**
 * Render the bottom right quartile of the Mandelbrot set.
 */
void render_bottom_right() {
    for (int x = screen_max_x / 2; x < screen_max_x; ++x) {
        for (int y = screen_max_y / 2; y < screen_max_y; ++y) {
            sf::Color current_pixel_color = calculate_point(x, y);
            screen[get_screen_pos(x, y)].color = current_pixel_color;
        }
    }
}

/**
 * Render the top left quartile of the Mandelbrot set.
 */
void render_top_left() {
    for (int x = screen_min_x; x < screen_max_x / 2; ++x) {
        for (int y = screen_min_y; y < screen_max_y / 2; ++y) {
            sf::Color current_pixel_color = calculate_point(x, y);
            screen[get_screen_pos(x, y)].color = current_pixel_color;
        }
    }
}

/**
 * Render the top right quartile of the Mandelbrot set.
 */
void render_top_right() {
    for (int x = screen_max_x / 2; x < screen_max_x; ++x) {
        for (int y = screen_min_y; y < screen_max_y / 2; ++y) {
            sf::Color current_pixel_color = calculate_point(x, y);
            screen[get_screen_pos(x, y)].color = current_pixel_color;
        }
    }
}

/**
 * Render the whole Mandelbrot set.
 */
static void render_mandelbrot() {
    // for (int x = screen_min_x; x < screen_max_x; ++x) {
    //     for (int y = screen_min_y; y < screen_max_y; ++y) {
    //         sf::Color current_pixel_color = calculate_point(x, y);
    //         screen[get_screen_pos(x, y)].color = current_pixel_color;
    //     }
    // }

    sf::Thread top_left(&render_top_left);
    sf::Thread top_right(&render_top_right);
    sf::Thread bottom_left(&render_bottom_left);
    sf::Thread bottom_right(&render_bottom_right);

    top_left.launch();
    top_right.launch();
    bottom_left.launch();
    bottom_right.launch();

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
