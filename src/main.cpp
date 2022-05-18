#include <iostream>
#include <complex>
#include <SFML/Graphics.hpp>
#include <omp.h>            // For multithreading
#include <math.h>           // For colors

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
#define SCREEN_X 1000
#define SCREEN_Y 600
#define MANDELBROT_X 1000
#define MANDELBROT_Y 800

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

// Class that maps the colors of the Mandelbrot set to the iteration 
class ColorMap {
    public:
        sf::Color get_color(unsigned int n) {
            // double smooth_color = ;
            // TODO: get smooth colors working
        }
};

// Class that actually renders the Mandelbrot set
class Mandelbrot : public sf::Drawable, public sf::Transformable {
    private:
        sf::VertexArray screen;
        int max_iterations;
        int screen_x;
        int screen_y;

        // Return the index in the VertexArray of a pixel
        int get_screen_pos(int x, int y) {
            return x + (this -> screen_x) * y;
        }

    protected:
        int get_convergence(std::complex<double> c) {
            int iteration = 0;
            std::complex<double> z(0., 0.);
            // While we are below the limit of iterations and don't diverge 
            while (iteration < (this -> max_iterations) && std::abs(z) < 2) {
                z = (z * z) + c;
                iteration++;
            }

            return iteration;   // Return the iteration on which we diverge 
        }

    public:
        // Mandelbrot set constructor
        Mandelbrot(int max_iter, int s_x, int s_y) {
            this -> max_iterations = max_iter;
            this -> screen.setPrimitiveType(sf::Points);        // Set the primitive type of the screen to pixels
            this -> screen.resize(s_x * s_y);         // Resize the screen to our desired screen dimensions
            this -> screen_x = s_x;                             // Update the private variables for the Mandelbrot's screen width and height
            this -> screen_y = s_y;
        }

        // Initial setup
        void update(std::complex<double> max, std::complex<double> min) {
            DEBUG_MSG("Updating Mandelbrot set.");
            #ifdef _OPENMP
                #pragma omp parallel for collapse(2) schedule(dynamic)
                for (int x = 0 ; x < this -> screen_x; ++x) {
                    for (int y = 0; y < this -> screen_y; ++y) {
                        // We create the 'c' complex number necessary for the formula
                        double c_r = min.real() + x * (max.real() - min.real()) / (this -> screen_x - 1);
                        double c_i = min.imag() + y * (max.imag() - min.imag()) / (this -> screen_y - 1);
                        std::complex<double> c(c_r, c_i);
                        int convergence_iteration = get_convergence(c);
                        int index = get_screen_pos(x, y);

                        this -> screen[index].position = sf::Vector2f(x, y);

                        if (convergence_iteration == this -> max_iterations) {
                            this -> screen[index].color = sf::Color::White;
                        } else {
                            this -> screen[index].color = sf::Color::Black;
                        }
                    }
                }
            #else
                for (int x = 0 ; x < this -> screen_x; ++x) {
                    double c_r = min.real() + x * (max.real() - min.real()) / (this -> screen_x - 1);
                    for (int y = 0; y < this -> screen_y; ++y) {
                        // We create the 'c' complex number necessary for the formula
                        double c_i = min.imag() + y * (max.imag() - min.imag()) / (this -> screen_y - 1);
                        std::complex<double> c(c_r, c_i);
                        int convergence_iteration = get_convergence(c);
                        int index = get_screen_pos(x, y);

                        this -> screen[index].position = sf::Vector2f(x, y);

                        if (convergence_iteration == this -> max_iterations) {
                            this -> screen[index].color = sf::Color::White;
                        } else {
                            this -> screen[index].color = sf::Color::Black;
                        }
                    }
                }
            #endif

            DEBUG_MSG("Initial setup completed.");
        }

    private:
        // Since we are inheriting from another class, we must define the virtual voids
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
            states.transform *= getTransform();
            states.texture = NULL;
            target.draw(this -> screen, states);
        }
};

int get_screen_pos(const int& x, const int& y) {
    return x + screen_max_x * (y - 1);
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

int main() {
    sf::RenderWindow window(sf::VideoMode(SCREEN_X, SCREEN_Y), "Fractal Generator");

    Mandelbrot mandelbrot(ITERATIONS, MANDELBROT_X, MANDELBROT_Y);

    std::complex<double> max(STARTING_FRACTAL_MAX_X, STARTING_FRACTAL_MAX_Y);
    std::complex<double> min(STARTING_FRACTAL_MIN_X, STARTING_FRACTAL_MIN_Y);

    bool init = true;
    
    while(window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (init) {
            window.clear(sf::Color::Black);
            window.display();
            mandelbrot.update(max, min);
            init = false;
        }

        window.clear(sf::Color::Black);
        window.draw(mandelbrot);        // Draw the Mandelbrot set itself by using the virtual void we already defined
        window.display();
    }

    return 0;
}
