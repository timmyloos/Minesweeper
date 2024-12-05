#include "GameWindow.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

sf::Texture numberTextures[8]; // Array to hold textures for numbers 1 to 8

const float TILE_SIZE = 32.0f;
const float BUTTON_SIZE = 32.0f;




GameWindow::GameWindow(int columns, int rows, int mines, const std::string& fontPath, const std::string& imagePath, const std::string& playerName)
    : window(sf::VideoMode(columns * TILE_SIZE, (rows * TILE_SIZE) + 100), "Minesweeper"),
      columns(columns), rows(rows), mines(mines), currentCounter(mines), playerName(playerName),
      leaderboard("files/font.ttf", "files/leaderboard.txt") { // Initialize leaderboard
    // Load font
    if (!font.loadFromFile(fontPath)) {
        std::cerr << "Failed to load font\n";
        exit(EXIT_FAILURE);
    }

    // Load textures
    if (!hiddenTexture.loadFromFile(imagePath + "/tile_hidden.png") ||
        !revealedTexture.loadFromFile(imagePath + "/tile_revealed.png") ||
        !mineTexture.loadFromFile(imagePath + "/mine.png") ||
        !flagTexture.loadFromFile(imagePath + "/flag.png") ||
        !digitsTexture.loadFromFile(imagePath + "/digits.png")) {
        std::cerr << "Failed to load base textures\n";
        exit(EXIT_FAILURE);
    }


    // Load number textures
    for (int i = 0; i < 8; ++i) {
        if (!numberTextures[i].loadFromFile(imagePath + "/number_" + std::to_string(i + 1) + ".png")) {
            std::cerr << "Failed to load texture for number " << (i + 1) << "\n";
            exit(EXIT_FAILURE);
        }
    }

    // Load button textures
    if (!happyFaceTexture.loadFromFile(imagePath + "/face_happy.png") ||
        !winFaceTexture.loadFromFile(imagePath + "/face_win.png") ||
        !loseFaceTexture.loadFromFile(imagePath + "/face_lose.png") ||
        !debugTexture.loadFromFile(imagePath + "/debug.png") ||
        !pauseTexture.loadFromFile(imagePath + "/pause.png") ||
        !playTexture.loadFromFile(imagePath + "/play.png") ||
        !leaderboardTexture.loadFromFile(imagePath + "/leaderboard.png")) {
        std::cerr << "Failed to load button textures\n";
        exit(EXIT_FAILURE);
    }

    // Initialize button sprites
    happyFace.setTexture(happyFaceTexture);
    debugButton.setTexture(debugTexture);
    pauseButton.setTexture(pauseTexture);
    leaderboardButton.setTexture(leaderboardTexture);

    // Set button positions
    happyFace.setPosition((columns * 32) / 2.0 - 32, 32 * (rows + 0.5));
    debugButton.setPosition((columns * 32) - 304, 32 * (rows + 0.5));
    pauseButton.setPosition((columns * 32) - 240, 32 * (rows + 0.5));
    leaderboardButton.setPosition((columns * 32) - 176, 32 * (rows + 0.5));

    // Initialize digit sprites for timer
    for (int i = 0; i < 3; ++i) { // Timer is displayed using 3 digits (max value 999)
        sf::Sprite digit(digitsTexture, sf::IntRect(0, 0, 21, 32)); // Initial texture rect for digit '0'
        digit.setPosition(((columns * TILE_SIZE) - 97) + i * 21, rows * TILE_SIZE + 16); // Timer digits' position
        timerDigits.push_back(digit);
    }

    // Initialize digit sprites for counter
    for (int i = 0; i < 3; ++i) { // Counter is displayed using 3 digits (max value 999)
        sf::Sprite digit(digitsTexture, sf::IntRect(0, 0, 21, 32)); // Initial texture rect for digit '0'
        digit.setPosition(33 + i * 21, (rows * TILE_SIZE) + 16); // Counter digits' position
        counterDigits.push_back(digit);
    }

    // Initialize the board and game logic
    createBoard();
    placeMines();
    calculateAdjacentMines();

    // Set initial counter and timer values
    updateCounterDisplay(currentCounter);
    updateTimerDisplay(0); // Start timer at 0
}




GameWindow::Tile::Tile(float x, float y, sf::Texture& hiddenTexture, sf::Texture& revealedTexture, sf::Texture* numberTextures)
    : backgroundSprite(hiddenTexture), numberTextures(numberTextures), isRevealed(false), isFlagged(false), isMine(false) {
    backgroundSprite.setPosition(x, y);
    sprite.setPosition(x, y);
}


void GameWindow::Tile::reveal(sf::Texture& revealedTexture, sf::Texture& mineTexture, sf::Texture* numberTextures, int number) {
    isRevealed = true;

    // Always update the background texture
    backgroundSprite.setTexture(revealedTexture);

    // Update the foreground texture and maintain position
    if (isMine) {
        sprite.setTexture(mineTexture); // Set mine texture
    } else if (number > 0) {
        sprite.setTexture(numberTextures[number - 1]); // Set number texture
    } else {
        sprite.setTexture(revealedTexture); // Blank tile for empty revealed tiles
    }

    // Ensure the foreground sprite maintains the correct position
    sprite.setPosition(backgroundSprite.getPosition());
    std::cout << "Tile revealed at position (" << row << ", " << col << ") with adjacent mines: " << number << "\n";
}


void GameWindow::revealNeighbors(Tile& tile) {
    // Mark the tile as revealed
    tile.isRevealed = true;

    int tileRow = tile.row;
    int tileCol = tile.col;

    std::cout << "Processing tile at (" << tileRow << ", " << tileCol << ")\n";

    // Iterate over the 8 possible neighbors
    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            // Skip the center tile itself
            if (dr == 0 && dc == 0) continue;

            int neighborRow = tileRow + dr;
            int neighborCol = tileCol + dc;

            // Skip out-of-bounds tiles
            if (neighborRow < 0 || neighborRow >= rows || neighborCol < 0 || neighborCol >= columns) {
                continue;
            }

            Tile& neighbor = board[neighborRow][neighborCol];

            // Skip already revealed or flagged tiles
            if (neighbor.isRevealed || neighbor.isFlagged) continue;

            // Reveal the neighbor
            neighbor.reveal(revealedTexture, mineTexture, numberTextures, neighbor.adjacentMines);
            std::cout << "Revealed neighbor at (" << neighborRow << ", " << neighborCol << ") with "
                      << neighbor.adjacentMines << " adjacent mines\n";

            // Recursively reveal neighbors if the current neighbor has no adjacent mines
            if (neighbor.adjacentMines == 0) {
                revealNeighbors(neighbor);
            }
        }
    }
}




void GameWindow::createBoard() {
    float tileSize = 32.0f; // Tile size
    for (int row = 0; row < rows; ++row) {
        std::vector<Tile> tileRow;
        for (int col = 0; col < columns; ++col) {
            Tile tile(col * tileSize, row * tileSize, hiddenTexture, revealedTexture, numberTextures);
            tile.row = row; // Explicitly set the row
            tile.col = col; // Explicitly set the column
            tile.backgroundSprite.setPosition(col * tileSize, row * tileSize);
            tile.sprite.setPosition(col * tileSize, row * tileSize);
            tileRow.push_back(tile);
        }
        board.push_back(tileRow);
    }
}







void GameWindow::placeMines() {
    std::srand(std::time(0)); // Seed for random generation
    int placedMines = 0;

    while (placedMines < mines) {
        int randomRow = std::rand() % rows;
        int randomCol = std::rand() % columns;

        // Check if the tile is already a mine
        if (!board[randomRow][randomCol].isMine) {
            board[randomRow][randomCol].isMine = true;

            // Assign mine texture to the tile
            board[randomRow][randomCol].sprite.setTexture(mineTexture);

            ++placedMines;
        }
    }
}

std::vector<GameWindow::Tile*> GameWindow::getNeighbors(int row, int col) {
    std::vector<GameWindow::Tile*> neighbors;

    // Iterate over the possible neighbor positions
    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue; // Skip the tile itself
            int neighborRow = row + dr;
            int neighborCol = col + dc;

            // Check if the neighbor is within the bounds of the grid
            if (neighborRow >= 0 && neighborRow < rows &&
                neighborCol >= 0 && neighborCol < columns) {
                neighbors.push_back(&board[neighborRow][neighborCol]);
                }
        }
    }

    return neighbors;
}






void GameWindow::revealAllMines() {
    for (auto& row : board) {
        for (auto& tile : row) {
            if (tile.isMine && !tile.isRevealed) {
                tile.reveal(revealedTexture, mineTexture, numberTextures);
                tile.isRevealed = true;
            }
        }
    }
}


bool GameWindow::checkWinCondition() {
    for (const auto& row : board) {
        for (const auto& tile : row) {
            if (!tile.isMine && !tile.isRevealed) {
                return false; // The player hasn't revealed all non-mine tiles
            }
        }
    }
    gameEnded = true;

    // Update the Happy Face to the sunglasses win face
    happyFace.setTexture(winFaceTexture);

    // Stop the timer
    elapsedTime = static_cast<int>(gameClock.getElapsedTime().asSeconds());

    // Add the player's time to the leaderboard
    leaderboard.update(playerName, elapsedTime);

    // Display the leaderboard
    leaderboard.display(window);

    return true;
}




void GameWindow::calculateAdjacentMines() {
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < columns; ++col) {
            if (board[row][col].isMine) continue;

            int mineCount = 0;
            for (int dr = -1; dr <= 1; ++dr) {
                for (int dc = -1; dc <= 1; ++dc) {
                    int r = row + dr, c = col + dc;
                    if (r >= 0 && r < rows && c >= 0 && c < columns && board[r][c].isMine) {
                        ++mineCount;
                    }
                }
            }
            board[row][col].adjacentMines = mineCount;
            std::cout << "Tile at (" << row << ", " << col << ") has " << mineCount << " adjacent mines.\n";
        }
    }
}




void GameWindow::handleLeftClick(Tile& tile) {
    if (gameEnded || tile.isFlagged || tile.isRevealed) return;

    if (tile.isMine) {
        tile.reveal(revealedTexture, mineTexture, numberTextures);
        revealAllMines();
        gameEnded = true;
        happyFace.setTexture(loseFaceTexture);
    } else {
        tile.reveal(revealedTexture, mineTexture, numberTextures, tile.adjacentMines);

        if (tile.adjacentMines == 0) {
            std::cout << "Tile has no adjacent mines. Revealing neighbors...\n";
            revealNeighbors(tile);
        }

        if (checkWinCondition()) {
            gameEnded = true;
            happyFace.setTexture(winFaceTexture);
        }
    }
}





void GameWindow::updateCounter(int value) {
    // Convert the value to a string (handle negatives)
    std::string valueStr = (value < 0 ? "-" : "") + std::to_string(std::abs(value));

    // Ensure the string has exactly 3 characters (pad with zeros if necessary)
    while (valueStr.size() < 3) {
        valueStr = "0" + valueStr;
    }

    // Render the digits (and handle the '-' sign if present)
    for (size_t i = 0; i < valueStr.size(); ++i) {
        if (valueStr[i] == '-') {
            counterDigits[i].setTextureRect(sf::IntRect(10 * 21, 0, 21, 32)); // '-' is the 11th sprite
        } else {
            int digitIndex = valueStr[i] - '0';
            counterDigits[i].setTextureRect(sf::IntRect(digitIndex * 21, 0, 21, 32));
        }
    }
}

void GameWindow::updateCounterDisplay(int counter) {
    if (counter > 999) counter = 999; // Cap counter at 999
    if (counter < -99) counter = -99; // Allow negative counters up to -99

    bool isNegative = (counter < 0);
    if (isNegative) counter = -counter;

    int hundreds = counter / 100;
    int tens = (counter / 10) % 10;
    int ones = counter % 10;

    if (isNegative) {
        counterDigits[0].setTextureRect(sf::IntRect(10 * 21, 0, 21, 32)); // '-' sprite
        counterDigits[1].setTextureRect(sf::IntRect(tens * 21, 0, 21, 32));
        counterDigits[2].setTextureRect(sf::IntRect(ones * 21, 0, 21, 32));
    } else {
        counterDigits[0].setTextureRect(sf::IntRect(hundreds * 21, 0, 21, 32));
        counterDigits[1].setTextureRect(sf::IntRect(tens * 21, 0, 21, 32));
        counterDigits[2].setTextureRect(sf::IntRect(ones * 21, 0, 21, 32));
    }
}




void GameWindow::handleRightClick(Tile& tile) {
    if (!tile.isRevealed) {
        tile.isFlagged = !tile.isFlagged;

        // Adjust the counter
        if (tile.isFlagged) {
            currentCounter--;
        } else {
            currentCounter++;
        }

        // Update the counter display
        updateCounterDisplay(currentCounter);

        tile.sprite.setTexture(tile.isFlagged ? flagTexture : hiddenTexture);
    }
}
void GameWindow::Tile::reset(const sf::Texture& hiddenTexture) {
    isRevealed = false;
    isFlagged = false;
    isMine = false;
    adjacentMines = 0;
    backgroundSprite.setTexture(hiddenTexture); // Reset background to hidden texture
    sprite.setTexture(hiddenTexture);          // Reset foreground to default
}



void GameWindow::resetGame() {
    // Reset game state
    gameEnded = false;
    paused = false;
    debugMode = false;

    // Reset timer
    elapsedTime = 0.0f;
    pauseTime = sf::Time::Zero;
    gameClock.restart();

    // Reset counter
    currentCounter = mines;
    updateCounterDisplay(currentCounter);

    // Reset board
    for (auto& row : board) {
        for (auto& tile : row) {
            tile.reset(hiddenTexture); // Pass the hiddenTexture to reset the tile
        }
    }

    // Place mines and calculate adjacent mine counts
    placeMines();
    calculateAdjacentMines();

    // Reset happy face button texture
    happyFace.setTexture(happyFaceTexture);

    std::cout << "Game reset successfully.\n";
}




void GameWindow::toggleDebugMode() {
    debugMode = !debugMode; // Toggle debug mode
    std::cout << "Debug mode toggled: " << (debugMode ? "ON" : "OFF") << std::endl;
}


void GameWindow::togglePause() {
    if (gameEnded) return; // Do nothing if the game has ended

    if (paused) {
        // Resume the game
        paused = false;
        pauseButton.setTexture(pauseTexture); // Switch to pause sprite
        gameClock.restart(); // Restart the clock
    } else {
        // Pause the game
        paused = true;
        pauseButton.setTexture(playTexture); // Switch to play sprite
        pauseTime = gameClock.getElapsedTime(); // Store elapsed time
    }

    std::cout << (paused ? "Game paused\n" : "Game resumed\n");
}




void GameWindow::showLeaderboard() {
    // Implement leaderboard display
    std::cout << "Leaderboard shown\n";
}

void GameWindow::handleMouseClick(const sf::Event::MouseButtonEvent& mouseButton) {
    sf::Vector2f mousePos(mouseButton.x, mouseButton.y);

    if (mouseButton.button == sf::Mouse::Left) {
        // Check for happy face button click
        if (happyFace.getGlobalBounds().contains(mousePos)) {
            std::cout << "Happy face clicked!\n";
            resetGame();
            return; // Exit function to avoid unnecessary checks
        }

        // Check for debug button click (only allowed if game is not paused)
        if (!paused && debugButton.getGlobalBounds().contains(mousePos)) {
            std::cout << "Debug button clicked!\n";
            toggleDebugMode();
            return;
        }

        // Check for pause button click
        if (pausePlayButton.getGlobalBounds().contains(mousePos)) {
            std::cout << (paused ? "Resume button clicked!\n" : "Pause button clicked!\n");
            togglePause();
            return;
        }

        // Check for leaderboard button click
        if (leaderboardButton.getGlobalBounds().contains(mousePos)) {
            if (!paused) togglePause(); // Automatically pause the game before showing leaderboard
            std::cout << "Leaderboard button clicked!\n";
            showLeaderboard();
            return;
        }

        // Handle tile clicks only if the game is not paused or ended
        if (!paused && !gameEnded) {
            for (auto& row : board) {
                for (auto& tile : row) {
                    if (tile.backgroundSprite.getGlobalBounds().contains(mousePos)) {
                        handleLeftClick(tile); // Handle left click on tile
                        return;
                    }
                }
            }
        }
    }
}


void GameWindow::Tile::toggleFlag(sf::Texture& flagTexture, sf::Texture& hiddenTexture) {
    isFlagged = !isFlagged;
    foregroundSprite.setTexture(isFlagged ? flagTexture : sf::Texture()); // Set flag texture if flagged, otherwise clear
}


void GameWindow::updateTimerDisplay(int time) {
    timerDigits.clear(); // Clear previous digit sprites

    // Calculate minutes and seconds
    int minutes = time / 60;
    int seconds = time % 60;

    // Convert to string with two digits for each
    std::string minuteStr = (minutes < 10 ? "0" : "") + std::to_string(minutes);
    std::string secondStr = (seconds < 10 ? "0" : "") + std::to_string(seconds);

    // Draw minute digits
    for (size_t i = 0; i < minuteStr.length(); ++i) {
        int digit = minuteStr[i] - '0';

        sf::Sprite digitSprite;
        digitSprite.setTexture(digitsTexture);
        digitSprite.setTextureRect(sf::IntRect(digit * 21, 0, 21, 32)); // Each digit is 21x32 pixels
        digitSprite.setPosition((columns * 32) - 97 + (i * 21), 32 * (rows + 0.5) + 16);
        timerDigits.push_back(digitSprite);
    }

    // Draw second digits
    for (size_t i = 0; i < secondStr.length(); ++i) {
        int digit = secondStr[i] - '0';

        sf::Sprite digitSprite;
        digitSprite.setTexture(digitsTexture);
        digitSprite.setTextureRect(sf::IntRect(digit * 21, 0, 21, 32)); // Each digit is 21x32 pixels
        digitSprite.setPosition((columns * 32) - 54 + (i * 21), 32 * (rows + 0.5) + 16);
        timerDigits.push_back(digitSprite);
    }
}




void GameWindow::run() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);

                // Check if the leaderboard button is clicked
                if (leaderboardButton.getGlobalBounds().contains(mousePos)) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        bool wasPaused = paused;  // Save the paused state
                        if (!paused) togglePause(); // Pause the game if not already paused
                        leaderboardOpen = true;
                        leaderboard.display(window);
                        leaderboardOpen = false;
                        if (!wasPaused) togglePause(); // Resume the game if it wasn't paused before
                        continue;
                    }
                }

                // Allow pause button interaction even if paused
                if (pauseButton.getGlobalBounds().contains(mousePos)) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        togglePause();
                        continue;
                    }
                }

                // Skip all other interactions if paused or leaderboard is open
                if (paused || leaderboardOpen) continue;

                // Handle other buttons
                if (happyFace.getGlobalBounds().contains(mousePos)) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        resetGame();
                        continue;
                    }
                }

                if (debugButton.getGlobalBounds().contains(mousePos)) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        toggleDebugMode();
                        continue;
                    }
                }

                // Check tile interactions
                for (auto& row : board) {
                    for (auto& tile : row) {
                        if (tile.backgroundSprite.getGlobalBounds().contains(mousePos)) {
                            if (event.mouseButton.button == sf::Mouse::Left) {
                                handleLeftClick(tile);
                            } else if (event.mouseButton.button == sf::Mouse::Right) {
                                handleRightClick(tile);

                                // Update the counter when a flag is toggled
                                updateCounterDisplay(currentCounter);
                            }
                        }
                    }
                }
            }
        }

        if (!paused && !gameEnded) {
            elapsedTime = pauseTime.asSeconds() + gameClock.getElapsedTime().asSeconds();
            updateTimerDisplay(static_cast<int>(elapsedTime)); // Update timer digits
        } else if (paused) {
            elapsedTime = pauseTime.asSeconds(); // Freeze the timer at the paused value
            updateTimerDisplay(static_cast<int>(elapsedTime)); // Display the frozen time
        }


        // Render everything
        window.clear(sf::Color::White);

        // Draw the board
        for (const auto& row : board) {
            for (const auto& tile : row) {
                window.draw(tile.backgroundSprite); // Draw background

                if (tile.isRevealed || tile.isFlagged) {
                    window.draw(tile.sprite); // Draw foreground if revealed or flagged
                }

                // If debug mode is active, reveal all mines
                if (debugMode && tile.isMine) {
                    window.draw(tile.sprite);
                }
            }
        }

        // Draw the counter digits
        for (const auto& digit : counterDigits) {
            window.draw(digit);
        }

        // Draw the timer digits
        for (const auto& digit : timerDigits) {
            window.draw(digit);
        }

        // Draw the buttons
        window.draw(happyFace);
        window.draw(debugButton);
        window.draw(pauseButton);
        window.draw(leaderboardButton);

        // Display everything
        window.display();
    }
}
