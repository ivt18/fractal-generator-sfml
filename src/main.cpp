#include <iostream>
#include <omp.h>            // For multithreading

// for the ColorMap class
#include "color_map.hpp"

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

// Class that actually renders the Mandelbrot set
class Mandelbrot : public sf::Drawable, public sf::Transformable {
    private:
        sf::VertexArray screen;
        unsigned int max_iterations;
        int screen_x;
        int screen_y;
        int move_x;
        int move_y;
        std::complex<double> max;
        std::complex<double> min;
        ColorMap cmap;

        // Return the index in the VertexArray of a pixel
        int get_screen_pos(int x, int y) {
            return x + (this -> screen_x) * y;
        }

    protected:
        int get_convergence(const std::complex<double>& c, std::complex<double>& z) {
            int iteration = 0;
            // While we are below the limit of iterations and don't diverge 
            while (iteration < (this -> max_iterations) && z.real() * z.real() + z.imag() * z.imag() < 4) {
                z = (z * z) + c;
                iteration++;
            }

            return iteration;   // Return the iteration on which we diverge 
        }

    public:
        // Mandelbrot set constructor
        Mandelbrot(const int& max_iterations_, const int& screen_x_, const int& screen_y_, const ColorMap& cmap_, const std::complex<double>& max_, const std::complex<double>& min_) {
            this -> max_iterations = max_iterations_;
            this -> screen.setPrimitiveType(sf::Points);        // Set the primitive type of the screen to pixels
            this -> screen.resize(screen_x_ * screen_y_);         // Resize the screen to our desired screen dimensions
            this -> screen_x = screen_x_;                             // Update the private variables for the Mandelbrot's screen width and height
            this -> screen_y = screen_y_;
            this -> cmap = cmap_;
            this -> move_x = 0;
            this -> move_y = 0;
            this -> max = max_;
            this -> min = min_;
        }

        // Initial setup
        void update() {
            DEBUG_MSG("Updating Mandelbrot set.");
            #ifdef COLOR_MAP_H
                #ifdef _OPENMP
                    #pragma omp parallel for collapse(2) schedule(dynamic)
                    for (int x = 0 ; x < this -> screen_x; ++x) {
                        for (int y = 0; y < this -> screen_y; ++y) {
                            // We create the 'c' complex number necessary for the formula
                            double c_r = this -> min.real() + (x + this -> move_x) * (this -> max.real() - this -> min.real()) / (this -> screen_x - 1);
                            double c_i = this -> min.imag() + (y + this -> move_y) * (this -> max.imag() - this -> min.imag()) / (this -> screen_y - 1);
                            std::complex<double> c(c_r, c_i);

                            std::complex<double> z_n(0., 0.);
                            unsigned int convergence_iteration = get_convergence(c, z_n);
                            int index = get_screen_pos(x, y);

                            this -> screen[index].position = sf::Vector2f(x, y);

                            // this -> screen[index].color = (convergence_iteration == this -> max_iterations) ? this -> cmap.get_color_expensive(convergence_iteration, z_n) : sf::Color::Black;
                            this -> screen[index].color = cmap.get_color_cheap(background_color, foreground_color, convergence_iteration, this -> max_iterations);
                        }
                    }
                #else
                    for (int x = 0 ; x < this -> screen_x; ++x) {
                        double c_r = this -> min.real() + (x + this -> move_x) * (this -> max.real() - this -> min.real()) / (this -> screen_x - 1);
                        for (int y = 0; y < this -> screen_y; ++y) {
                            // We create the 'c' complex number necessary for the formula
                            double c_i = this -> min.imag() + (y + this -> move_y) * (this -> max.imag() - this -> min.imag()) / (this -> screen_y - 1);
                            std::complex<double> c(c_r, c_i);
                            std::complex<double> z_n(0., 0.);
                            unsigned int convergence_iteration = get_convergence(c, z_n);
                            int index = get_screen_pos(x, y);

                            this -> screen[index].position = sf::Vector2f(x, y);

                            // this -> screen[index].color = (convergence_iteration == this -> max_iterations) ? this -> cmap.get_color_expensive(convergence_iteration, z_n) : sf::Color::Black;
                            // TODO: openmp cannot use functions like this that are from other classes. therefore, just define the mandelbrot class and put this function in there
                            this -> screen[index].color = cmap.get_color_cheap(background_color, foreground_color, convergence_iteration, this -> max_iterations);
                        }
                    }
                #endif
            #endif

            DEBUG_MSG("Initial setup completed.");
        }

        /**
         * Move the fractal by a few pixels.
         * @param pixel_x : pixels to move in 'x' axis
         * @param pixel_y : pixels to move in 'y' axis
         */
        void move_fractal(const int& pixel_x, const int& pixel_y) {
            this -> move_x -= pixel_x;
            this -> move_y -= pixel_y;
            this -> update();
        }

        /**
         * Zoom in by removing a few pixels from each side, while still maintaining the aspect ratio.
         * @param zoom_pixels : pixels to zoom in by
         */
        void zoom_fractal(const int& zoom_pixels) {
            const std::complex<double> old_max = max;
            const std::complex<double> old_min = min;
            this -> max = std::complex<double>(old_max.real() - zoom_pixels * (old_max.real() - old_min.real()) / (this -> screen_x - 1),
                    old_max.imag() - zoom_pixels * ((double)this -> screen_y / (double)this -> screen_x) * (old_max.imag() - old_min.imag()) / (this -> screen_y - 1));     // Zoom in from top right corner
            this -> min = std::complex<double>(old_min.real() + zoom_pixels * (old_max.real() - old_min.real()) / (this -> screen_x - 1),
                    old_min.imag() + zoom_pixels * ((double)this -> screen_y / (double)this -> screen_x) * (old_max.imag() - old_min.imag()) / (this -> screen_y - 1));     // Zoom in from bottom left corner
            this -> update();
        }

    private:
        // Since we are inheriting from another class, we must define the virtual voids
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
            states.transform *= getTransform();
            states.texture = NULL;
            target.draw(this -> screen, states);
        }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(SCREEN_X, SCREEN_Y), "Fractal Generator");

    std::complex<double> max(STARTING_FRACTAL_MAX_X, STARTING_FRACTAL_MAX_Y);
    std::complex<double> min(STARTING_FRACTAL_MIN_X, STARTING_FRACTAL_MIN_Y);

    ColorMap cmap;
    Mandelbrot mandelbrot(ITERATIONS, MANDELBROT_X, MANDELBROT_Y, cmap, max, min);

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
