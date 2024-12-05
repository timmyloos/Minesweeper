#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H


#include "LeaderBoard.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class GameWindow {
public:
    GameWindow(int columns, int rows, int mines, const std::string& fontPath, const std::string& imagePath, const std::string& playerName);
    void run();
    void resetGame();                    // Resets the game
    void toggleDebugMode();              // Toggles debug mode
    void togglePause();                  // Toggles pause/resume
    void showLeaderboard();              // Displays the leaderboard
    void handleMouseClick(const sf::Event::MouseButtonEvent& mouseButton); // Handles button clicks
    void updateTimerDisplay(int time);
    void updateCounter(int value);
    void updateCounterDisplay(int counter);
    bool checkWinCondition();
    void revealAllMines();



private:
    Leaderboard leaderboard; // Instance of the Leaderboard class
    sf::RenderWindow window;
    sf::Font font;

    // Button textures and sprites
    sf::Texture happyFaceTexture, winFaceTexture, loseFaceTexture, debugTexture, pauseTexture, playTexture, leaderboardTexture;
    sf::Sprite happyFace, debugButton, pauseButton, leaderboardButton, pausePlayButton;

    // Tile textures
    sf::Texture hiddenTexture, revealedTexture, mineTexture, flagTexture, digitsTexture;

    // Timer-related members
    sf::Clock gameClock, timer;
    sf::Time pauseTime, timerText;
    std::vector<sf::Sprite> timerDigits;   // Sprites for each digit in the timer
    int elapsedTime = 0;                   // Timer value in seconds
    float elapsedTimeOffset;  // Accumulates paused time


    // Game state flags
    bool debugMode = false;                // Track debug mode state
    bool paused = false;                   // Track whether the game is paused
    bool gameEnded = false;                // Track whether the game has ended (win/loss)

    int currentCounter;
    std::vector<sf::Sprite> counterDigits;
    bool leaderboardOpen = false;  // Track if the leaderboard is open
    sf::RectangleShape leaderboardWindow;  // A simple placeholder for the leaderboard UI
    bool playerWon = false; // New flag for winning
    bool playerLost = false;
    std::string playerName;
    int tileSize; // Tile size in pixels



    struct Tile {
        sf::Sprite backgroundSprite; // Background for the tile
        sf::Sprite foregroundSprite; // Foreground (number or mine)
        sf::Sprite sprite;
        bool isRevealed = false;
        bool isFlagged = false;
        bool isMine = false;
        int adjacentMines = 0;
        int row = 0, col = 0;
        sf::Texture* numberTextures = nullptr;

        // Updated constructor to accept the numberTextures array

        Tile(float x, float y, sf::Texture& hiddenTexture, sf::Texture& revealedTexture, sf::Texture* numberTextures);

        void reveal(sf::Texture& revealedTexture, sf::Texture& mineTexture, sf::Texture* numberTextures, int number = 0);
        void toggleFlag(sf::Texture& flagTexture, sf::Texture& hiddenTexture);
        void reset(const sf::Texture& hiddenTexture);
    };



    std::vector<std::vector<Tile>> board;
    int columns;
    int rows;
    int mines;

    void createBoard();
    void placeMines();
    void calculateAdjacentMines();

    void revealNeighbors(Tile& tile);
    void handleLeftClick(Tile& tile);
    void handleRightClick(Tile& tile);
    std::vector<Tile*> getNeighbors(int row, int col);
};

#endif // GAME_WINDOW_H
