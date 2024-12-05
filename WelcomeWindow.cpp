#include "WelcomeWindow.h"
#include <cctype>
#include <iostream>

WelcomeWindow::WelcomeWindow(const std::string& fontPath)
    : window(sf::VideoMode(800, 612), "Welcome to Minesweeper") {
    if (!font.loadFromFile(fontPath)) {
        std::cerr << "Failed to load font\n";
        exit(EXIT_FAILURE);
    }

    // Initialize title
    title.setFont(font);
    title.setString("WELCOME TO MINESWEEPER!");
    title.setCharacterSize(24);
    title.setStyle(sf::Text::Bold | sf::Text::Underlined);
    title.setFillColor(sf::Color::White);
    centerText(title, 400, 150);

    // Initialize prompt
    prompt.setFont(font);
    prompt.setString("Enter your name:");
    prompt.setCharacterSize(20);
    prompt.setFillColor(sf::Color::White);
    centerText(prompt, 400, 250);

    // Initialize name text
    nameText.setFont(font);
    nameText.setCharacterSize(18);
    nameText.setFillColor(sf::Color::Yellow);
    nameText.setString("");
    centerText(nameText, 400, 300);

    // Initialize cursor
    cursor.setFont(font);
    cursor.setString("|");
    cursor.setCharacterSize(18);
    cursor.setFillColor(sf::Color::White);
    cursor.setOrigin(0, cursor.getLocalBounds().height / 2.0f); // Center cursor vertically
}


void WelcomeWindow::centerText(sf::Text& text, float x, float y) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
    text.setPosition(x, y);
}

bool WelcomeWindow::run(std::string& playerName) {
    bool launchGame = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return false; // Prevent launching the game
            } else if (event.type == sf::Event::TextEntered) {
                char enteredChar = static_cast<char>(event.text.unicode);
                if (std::isalpha(enteredChar) && playerName.size() < 10) {
                    if (playerName.empty()) enteredChar = std::toupper(enteredChar);
                    else enteredChar = std::tolower(enteredChar);
                    playerName += enteredChar;
                }
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::BackSpace && !playerName.empty()) {
                    playerName.pop_back();
                }
                if (event.key.code == sf::Keyboard::Enter && !playerName.empty()) {
                    launchGame = true;
                    window.close();
                }
            }
        }

        // Update name text and dynamically center it
        nameText.setString(playerName);
        centerText(nameText, 400, 300);

        // Get updated local bounds of nameText
        sf::FloatRect nameBounds = nameText.getLocalBounds();

        // Calculate the right edge of nameText
        float nameTextRightEdge = nameText.getPosition().x + (nameBounds.width / 2.0f);

        // Position the cursor at the end of the nameText
        cursor.setPosition(nameTextRightEdge + 5, nameText.getPosition().y);

        // Draw window
        window.clear(sf::Color::Blue);
        window.draw(title);
        window.draw(prompt);
        window.draw(nameText);
        window.draw(cursor);
        window.display();
    }

    return launchGame;
}
