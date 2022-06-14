#ifndef COLOR_MAP_H
#define COLOR_MAP_H

// for complex numbers
#include <complex>
// for SFML graphics
#include <SFML/Graphics.hpp>
// for colors
#include <math.h>

// Class that maps the colors of the Mandelbrot set to the iteration 
class ColorMap {
private:
    // convert HSB color to RGB
    sf::Color hsv_to_rgb(double smooth_color, double saturation, double value);

public:
    // get a color for a certain iteration 'n' using logarithmic interpolation; too expensive
    sf::Color get_color_expensive(unsigned int n, std::complex<double> z);


    // get a color for a certain iteration 'n' using simple linear interpolation
    sf::Color get_color_cheap(sf::Color background_color_, sf::Color foreground_color_, unsigned int convergence_iteration, unsigned int max_iterations);
};

#endif // COLOR_MAP_H
