#include <iostream>
#include <SFML/Graphics.hpp>

// Debugging
#define DEBUG 1
#ifdef DEBUG
#define DEBUG_MSG(str) do { std::cout << str << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif

#define SCREEN_X 800
#define SCREEN_Y 600

const int min_x = 0;                                        // The minimum 'x' coordinate of a visible pixel
const int min_y = 1;                                        // The minimum 'y' coordinate of a visible pixel
const int max_x = SCREEN_X - 1;                             // The maximum 'x' coordinate of a visible pixel
const int max_y = SCREEN_Y;                                 // The maximum 'y' coordinate of a visible pixel

sf::VertexArray screen(sf::Points, SCREEN_X * SCREEN_Y);

/**
 * Initialize the pixel's positions and colors. 
 * Only done at the start.
 */
static void init_pixels() {
    for (int x = min_x; x <= max_x; ++x) {                                      // For every 'x' coordinate
        for (int y = min_y; y <= max_y; ++y) {                                  // For every 'y' coordinate 
            screen[x + max_x * (y - 1)].position = sf::Vector2f(x, y);          // We set the corresponding pixel's position
            screen[x + max_x * (y - 1)].color = sf::Color::Black;               // And we make it black
        }
    }

    DEBUG_MSG("Pixel initialization completed.");
}

int main() {
    sf::RenderWindow window(sf::VideoMode(SCREEN_X, SCREEN_Y), "Fractal Generator");

    init_pixels();
    
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
