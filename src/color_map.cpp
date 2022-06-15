#include "color_map.hpp"

ColorMap::ColorMap() {}

/**
 * Class constructor.
 * @param background_color_ the background color of the fractal 
 * @param foreground_color_ the foreground color of the fractal 
 */
ColorMap::ColorMap(const sf::Color& background_color_, const sf::Color& foreground_color_) {
    background_color = background_color_;
    foreground_color = foreground_color_;
}

/**
 * Convert hSB color to RGB 
 * @param smooth_color 
 * @param saturation 
 * @param value 
 */
sf::Color ColorMap::hsv_to_rgb(double smooth_color, double saturation, double value) {
    int H = 0.95 + 20 * smooth_color;
    double S = saturation;
    double V = value;

    // If the hue is outside the necessary limit of a single circumference
    while (H < 0) {
        H += 360;
    }
    while (H > 360) {
        H -= 360;
    }

    double C = V * S;
    double X = C * (1 - std::abs((H / 60) % 2 - 1));
    double m = V - C;

    double R_, G_, B_;

    if ((H >= 0 && H < 60)) {
        R_ = C;
        G_ = X;
        B_ = 0;
    }

    if (H >= 60 && H < 120) {
        R_ = X;
        G_ = C;
        B_ = 0;
    }

    if (H >= 120 && H < 180) {
        R_ = 0;
        G_ = C;
        B_ = X;
    }

    if (H >= 180 && H < 240) {
        R_ = 0;
        G_ = X;
        B_ = C;
    }

    if (H >= 240 && H < 300) {
        R_ = X;
        G_ = 0;
        B_ = C;
    }

    if (H >= 300 && H < 360) {
        R_ = C;
        G_ = 0;
        B_ = X;
    }

    uint8_t R = (int)((R_ + m) * 255);
    uint8_t G = (int)((G_ + m) * 255);
    uint8_t B = (int)((B_ + m) * 255);

    return sf::Color(R, G, B);
}

/**
 * Get a color for a certain iteration using logarithmic interpolation
 * @param n
 * @param z
 */
sf::Color ColorMap::get_color_expensive(unsigned int n, std::complex<double> z) {
    double smooth_color = n + 1 - std::log(std::log(std::abs(z))) / std::log(2);
    return hsv_to_rgb(smooth_color, 0.8, 1.0);
}

/**
 * Get a color for a certain iteration using simple linear interpolation 
 * @param background_color_ 
 * @param foreground_color_
 * @param convergence_iteration 
 * @param max_iterations 
 */
sf::Color ColorMap::get_color_cheap(unsigned int convergence_iteration, unsigned int max_iterations) {
    double p = (double)convergence_iteration / (double)max_iterations;
    double r = p * (foreground_color.r - background_color.r) + background_color.r;
    double g = p * (foreground_color.g - background_color.g) + background_color.g;
    double b = p * (foreground_color.b - background_color.b) + background_color.b;

    return sf::Color(r, g, b);
}
