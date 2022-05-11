#include <iostream>
#include <SFML/Graphics.hpp>

#define SCREEN_X 800
#define SCREEN_Y 600

int main() {
    sf::RenderWindow window(sf::VideoMode(SCREEN_X, SCREEN_Y), "Fractal Generator");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);
    
    while(window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}
