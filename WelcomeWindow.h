#ifndef WELCOME_WINDOW_H
#define WELCOME_WINDOW_H

#include <SFML/Graphics.hpp>
#include <string>

class WelcomeWindow {
public:
    WelcomeWindow(const std::string& fontPath);
    bool run(std::string& playerName);

private:
    sf::RenderWindow window;
    sf::Font font;
    sf::Text title;
    sf::Text prompt;
    sf::Text nameText;
    sf::Text cursor;

    void centerText(sf::Text& text, float x, float y);
};

#endif // WELCOME_WINDOW_H