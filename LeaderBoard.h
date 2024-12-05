#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Leaderboard {
public:
    Leaderboard(const std::string& fontPath, const std::string& leaderboardFile);
    void display(sf::RenderWindow& parentWindow); // Display the leaderboard
    void update(const std::string& playerName, int time); // Update leaderboard with a new score

private:
    sf::Font font;
    sf::Text titleText;            // "LEADERBOARD"
    std::vector<sf::Text> entries; // Leaderboard entries
    std::string leaderboardFile;
    std::vector<std::pair<int, std::string>> scores; // Pair of time and player name

    void loadFromFile();      // Load leaderboard data from file
    void saveToFile();        // Save updated leaderboard data to file
    void formatEntries();     // Format the text objects for display
};

#endif // LEADERBOARD_H
