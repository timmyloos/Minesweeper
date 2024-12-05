#include "LeaderBoard.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>

Leaderboard::Leaderboard(const std::string& fontPath, const std::string& leaderboardFile)
    : leaderboardFile(leaderboardFile) {
    // Load the font
    if (!font.loadFromFile(fontPath)) {
        std::cerr << "Failed to load font from: " << fontPath << std::endl;
        exit(EXIT_FAILURE);
    }

    titleText.setFont(font);
    titleText.setString("LEADERBOARD");
    titleText.setCharacterSize(20); // Font size for title
    titleText.setStyle(sf::Text::Bold | sf::Text::Underlined);
    titleText.setFillColor(sf::Color::White);


    // Load leaderboard data from file
    loadFromFile();
    formatEntries();
}

void Leaderboard::display(sf::RenderWindow& parentWindow) {
    sf::RenderWindow leaderboardWindow(sf::VideoMode(400, 300), "Leaderboard", sf::Style::Close);
    titleText.setPosition(200 - (titleText.getGlobalBounds().width / 2), 20); // Center title

    while (leaderboardWindow.isOpen()) {
        sf::Event event;
        while (leaderboardWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                leaderboardWindow.close();
            }
        }

        leaderboardWindow.clear(sf::Color::Blue);

        // Draw the title and entries
        leaderboardWindow.draw(titleText);
        for (const auto& entry : entries) {
            leaderboardWindow.draw(entry);
        }

        leaderboardWindow.display();
    }
}

void Leaderboard::loadFromFile() {
    std::ifstream file(leaderboardFile);
    if (!file.is_open()) {
        std::cerr << "Failed to open leaderboard file: " << leaderboardFile << std::endl;
        return;
    }

    scores.clear();
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string timeStr, playerName;
        if (std::getline(iss, timeStr, ',') && std::getline(iss, playerName)) {
            int minutes = std::stoi(timeStr.substr(0, 2)); // Extract minutes
            int seconds = std::stoi(timeStr.substr(3, 2)); // Extract seconds
            int totalSeconds = minutes * 60 + seconds;
            scores.push_back({totalSeconds, playerName});
        }
    }
    file.close();
}



void Leaderboard::saveToFile() {
    std::ofstream file(leaderboardFile, std::ofstream::trunc); // Open file for writing
    if (!file.is_open()) {
        std::cerr << "Failed to open leaderboard file for writing: " << leaderboardFile << std::endl;
        return;
    }

    int rank = 0; // Track rank to ensure only the top 5 players are written
    for (const auto& score : scores) {
        if (rank++ >= 5) break; // Stop writing after 5 players

        std::string playerName = score.second;
        size_t asteriskPos = playerName.find('*');
        if (asteriskPos != std::string::npos) {
            // Remove asterisk from the name
            playerName = playerName.substr(0, asteriskPos);
        }

        // Write to file in the format "MM:SS,PlayerName"
        int minutes = score.first / 60;
        int seconds = score.first % 60;
        file << std::setw(2) << std::setfill('0') << minutes << ":"
             << std::setw(2) << std::setfill('0') << seconds << ","
             << playerName << "\n";
    }

    file.close();
}


void Leaderboard::formatEntries() {
    entries.clear();
    int rank = 1;
    float yOffset = 60.0f;

    for (const auto& score : scores) {
        sf::Text entry;
        entry.setFont(font);

        // Format rank, time in MM:SS, and player name
        std::ostringstream oss;
        int minutes = score.first / 60; // Convert total seconds to minutes
        int seconds = score.first % 60; // Convert total seconds to seconds
        oss << rank++ << ". "
            << std::setw(2) << std::setfill('0') << minutes << ":" // Format MM
            << std::setw(2) << std::setfill('0') << seconds << " " // Format SS
            << score.second; // Include name and asterisk

        entry.setString(oss.str());
        entry.setCharacterSize(18);
        entry.setFillColor(sf::Color::White);

        // Center the entry text
        sf::FloatRect textBounds = entry.getLocalBounds();
        entry.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
        entry.setPosition(200, yOffset);

        entries.push_back(entry);
        yOffset += 30.0f; // Space between entries
    }
}

void Leaderboard::update(const std::string& playerName, int time) {
    std::cout << "Updating leaderboard with: " << playerName << ", " << time << " seconds\n";

    // Add new score
    scores.push_back({time, playerName});

    // Sort scores in ascending order of time
    std::sort(scores.begin(), scores.end(), [](const std::pair<int, std::string>& a, const std::pair<int, std::string>& b) {
        return a.first < b.first; // Sort by time
    });

    // Keep only the top 5 scores
    if (scores.size() > 5) {
        scores.resize(5);
    }

    // Mark the new score with an asterisk (*)
    for (auto& score : scores) {
        if (score.second == playerName && score.first == time) {
            score.second += " *"; // Add asterisk
            break;
        }
    }

    // Save updated scores and reformat entries
    saveToFile();
    formatEntries();
}
