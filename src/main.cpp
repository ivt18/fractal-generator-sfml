#include <iostream>
#include <complex>
#include <SFML/Graphics.hpp>

// Debugging
// #define DEBUG 1
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

// For panning around the Mandelbrot set
#define SENSITIVITY 2
double move_x = 0;          // We start with no movement on any of the axis
double move_y = 0;

// For complex numbers
const std::complex<double> i(0., 1.);       // The 'i' complex number

sf::VertexArray screen(sf::Points, SCREEN_X * SCREEN_Y);

class Mandelbrot : public sf::Drawable, public sf::Transformable {
    private:
        sf::VertexArray screen;
        int max_iterations;
        int screen_x;
        int screen_y;
        std::complex<double> min;
        std::complex<double> max;

        // Return the index in the VertexArray of a pixel
        int get_screen_pos(int x, int y) {
            return x + this -> screen_x * (y - 1);
        }

    protected:
        int get_convergence(std::complex<double> c) {
            int iteration;
            std::complex<double> z(0., 0.);
            // While we are below the limit of iterations and don't diverge 
            while (iteration < this -> max_iterations && std::real(z) * std::real(z) + std::imag(z) + std::imag(z) < 4) {
                z = z * z + c;
                iteration++;
            }

            return iteration;   // Return the iteration on which we diverge 
        }

    public:
        // Mandelbrot set constructor
        Mandelbrot(int max_iterations, int screen_x, int screen_y) {
            this -> max_iterations = max_iterations;
            this -> screen.resize(screen_x * screen_y);
            this -> screen_x = screen_x;
            this -> screen_y = screen_y;
        }

        // Initial setup
        void init() {
            min = std::complex<double>(STARTING_FRACTAL_MIN_X, STARTING_FRACTAL_MIN_Y);
            max = std::complex<double>(STARTING_FRACTAL_MAX_X, STARTING_FRACTAL_MAX_Y);
            for (int x = 0 ; x <= this -> screen_x; ++x) {
                for (int y = 0; y <= this -> screen_y; ++y) {
                    // We create the 'c' complex number necessary for the formula
                    double c_r = this -> min.real() + x * (this -> max.real() - this -> min.real()) / (this -> screen_x - 1);
                    double c_i = this -> min.imag() + y * (this -> max.imag() - this -> min.imag()) / (this -> screen_y - 1);
                    std::complex<double> c(c_r, c_i);
                    int convergence_iteration = get_convergence(c);
                    int index = get_screen_pos(x, y);

                    if (convergence_iteration = this -> max_iterations) {
                        this -> screen[index].color = sf::Color::White;
                    } else {
                        this -> screen[index].color = sf::Color::Black;
                    }

                    this -> screen[index].position = sf::Vector2f(x, y);
                }
            }
        }
};

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
    z_x = fractal_min_x + (x + move_x * SENSITIVITY) * x_pixel;
    z_y = fractal_max_y - (y + move_y * SENSITIVITY) * y_pixel;

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

// For multithreading
sf::Thread top_left(&render_top_left);
sf::Thread top_right(&render_top_right);
sf::Thread bottom_left(&render_bottom_left);
sf::Thread bottom_right(&render_bottom_right);

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

    top_left.launch();
    top_right.launch();
    bottom_left.launch();
    bottom_right.launch();

    DEBUG_MSG("Mandelbrot set rendered.");
}

void read_screen_dragging(sf::RenderWindow* window) {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {              // If we are pressing the left mouse button
        sf::Vector2i initial_position = sf::Mouse::getPosition(*window);           // Get the mouse's position relative ot the window
        // sf::sleep(sf::milliseconds(50));            // Sleep the thread for a bit
        sf::Vector2i new_position = sf::Mouse::getPosition(*window);
        move_x -= new_position.x - initial_position.x;
        move_y -= new_position.y - initial_position.y;
        render_mandelbrot();
    }
}


int main() {
    sf::RenderWindow window(sf::VideoMode(SCREEN_X, SCREEN_Y), "Fractal Generator");

    init_pixels();

    render_mandelbrot();
    
    while(window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::MouseMoved) {   // If we are moving the mouse
                read_screen_dragging(&window);
            }

            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(screen);
        window.display();
    }

    return 0;
}
