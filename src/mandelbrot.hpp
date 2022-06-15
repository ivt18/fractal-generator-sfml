#ifndef MANDELBROT_H
#define MANDELBROT_H

// for multithreading
#include <omp.h>
// for the ColorMap class
#include "color_map.hpp"

// Debugging
// #define DEBUG 1
#ifdef DEBUG
#define DEBUG_MSG(str) do { std::cout << str << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif

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

    // return the index in the VertexArray of a specific pixel on the screen
    int get_screen_pos(int x, int y);

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

protected:
    // return the iteration on which we diverge
    int get_convergence(const std::complex<double>& c, std::complex<double>& z);

public:
    Mandelbrot(const int& max_iterations_, const int& screen_x_, const int& screen_y_, 
            const std::complex<double>& max_, const std::complex<double>& min_,
            const ColorMap& cmap_);

    void update();

    void move_fractal(const int& pixel_x, const int& pixel_y);

    void zoom_fractal(const int& zoom_pixels);
};

#endif // MANDELBROT_H
