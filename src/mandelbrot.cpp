#include "mandelbrot.hpp"

// TODO: still gotta describe some of the function parameters

int Mandelbrot::get_convergence(const std::complex<double>& c, std::complex<double>& z) {
    int iteration = 0;
    // While we are below the limit of iterations and don't diverge 
    while (iteration < (max_iterations) && z.real() * z.real() + z.imag() * z.imag() < 4) {
        z = (z * z) + c;
        iteration++;
    }

    return iteration;   // Return the iteration on which we diverge 
}

/**
 * Mandelbrot set constrcutor
 * @param max_iterations_ the maximum number of iterations we go through before we consdier divergence 
 * @param screen_x_ the screen x-dimension (screen space)
 * @param screen_y_ the screen y-dimension (screen space)
 * @param max_ the maximum (x, y) coordinates of the fractal (mathematical, world space)
 * @param min_ the minimum (x, y) coordinates of the fractal (mathematical, world space)
 * @param background_color_ the background color of the fractal 
 * @param foreground_color_ the foreground color of the fractal
 */
Mandelbrot::Mandelbrot(const int& max_iterations_, const int& screen_x_, const int& screen_y_, 
        const std::complex<double>& max_, const std::complex<double>& min_,
        const ColorMap& cmap_) {
    max_iterations = max_iterations_;
    screen.setPrimitiveType(sf::Points);            // Set the primitive type of the screen to pixels
    screen.resize(screen_x_ * screen_y_);           // Resize the screen to our desired screen dimensions
    screen_x = screen_x_;                           // Update the private variables for the Mandelbrot's screen width and height
    screen_y = screen_y_;
    move_x = 0;
    move_y = 0;
    max = max_;
    min = min_;
    cmap = cmap_;
}


/**
 * Return the index in the VertexArray of a pixel
 * @param x the x-coordinate of the pixel 
 * @param y the y=coordinate of the pixel
 */
int Mandelbrot::get_screen_pos(int x, int y) {
    return x + screen_x * y;
}

/**
 * Update the fractal. 
 */
void Mandelbrot::update() {
    DEBUG_MSG("Updating Mandelbrot set.");
    #ifdef _OPENMP
        #pragma omp parallel for collapse(2) schedule(dynamic)
        for (int x = 0 ; x < screen_x; ++x) {
            for (int y = 0; y < screen_y; ++y) {
                // We create the 'c' complex number necessary for the formula
                double c_r = min.real() + (x + move_x) * (max.real() - min.real()) / (screen_x - 1);
                double c_i = min.imag() + (y + move_y) * (max.imag() - min.imag()) / (screen_y - 1);
                std::complex<double> c(c_r, c_i);

                std::complex<double> z_n(0., 0.);
                unsigned int convergence_iteration = get_convergence(c, z_n);
                int index = get_screen_pos(x, y);

                screen[index].position = sf::Vector2f(x, y);

                // screen[index].color = (convergence_iteration == max_iterations) ? cmap.get_color_expensive(convergence_iteration, z_n) : sf::Color::Black;
                screen[index].color = cmap.get_color_cheap(convergence_iteration, max_iterations);
            }
        }
    #else
        for (int x = 0 ; x < screen_x; ++x) {
            double c_r = min.real() + (x + move_x) * (max.real() - min.real()) / (screen_x - 1);
            for (int y = 0; y < screen_y; ++y) {
                // We create the 'c' complex number necessary for the formula
                double c_i = min.imag() + (y + move_y) * (max.imag() - min.imag()) / (screen_y - 1);
                std::complex<double> c(c_r, c_i);
                std::complex<double> z_n(0., 0.);
                unsigned int convergence_iteration = get_convergence(c, z_n);
                int index = get_screen_pos(x, y);

                screen[index].position = sf::Vector2f(x, y);

                // screen[index].color = (convergence_iteration == max_iterations) ? cmap.get_color_expensive(convergence_iteration, z_n) : sf::Color::Black;
                screen[index].color = cmap.get_color_cheap(convergence_iteration, max_iterations);
            }
        }
    #endif

    DEBUG_MSG("Initial setup completed.");
}

/**
 * Move the fractal by a few pixels.
 * @param pixel_x pixels to move in 'x' axis
 * @param pixel_y pixels to move in 'y' axis
 */
void Mandelbrot::move_fractal(const int& pixel_x, const int& pixel_y) {
    move_x -= pixel_x;
    move_y -= pixel_y;
    update();
}


/**
 * Zoom in by removing a few pixels from each side, while still maintaining the aspect ratio.
 * @param zoom_pixels pixels to zoom in by
 */
void Mandelbrot::zoom_fractal(const int& zoom_pixels) {
    const std::complex<double> old_max = max;
    const std::complex<double> old_min = min;
    max = std::complex<double>(old_max.real() - zoom_pixels * (old_max.real() - old_min.real()) / (screen_x - 1),
            old_max.imag() - zoom_pixels * ((double)screen_y / (double)screen_x) * (old_max.imag() - old_min.imag()) / (screen_y - 1));     // Zoom in from top right corner
    min = std::complex<double>(old_min.real() + zoom_pixels * (old_max.real() - old_min.real()) / (screen_x - 1),
            old_min.imag() + zoom_pixels * ((double)screen_y / (double) screen_x) * (old_max.imag() - old_min.imag()) / (screen_y - 1));     // Zoom in from bottom left corner
    update();
}


// Since we are inheriting from another class, we must define the virtual voids
void Mandelbrot::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    states.texture = NULL;
    target.draw(screen, states);
}
