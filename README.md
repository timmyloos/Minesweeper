<!--
Name: Timmy Loos
Section: #11969
UFL email: tloos@ufl.edu
System: macOS
SFML version: SFML 2.5
IDE: CLion
Other notes: Didn't use config, columns, rows, and mines can be changed in main.
-->
# Minesweeper 
#### Fall 2024 Project
> 
Compiler: g++
>
SFML version: SFML 2.5
>
>If you are a student in COP3503C at the University of Florida, you are strongly advised not to submit or use this code as your own work. Doing so would constitute plagiarism, which can result in severe academic consequences. Use this repository only for learning and reference purposes.
## About 
This project is a custom Minesweeper game built using C++ and SFML (Simple and Fast Multimedia Library). It replicates the classic Minesweeper experience, including core functionalities like mine placement, tile revealing, flagging, win/loss conditions, a leaderboard system, and game state management (pause, reset, etc.).
>
The game consists of a graphical user interface (GUI) where the player interacts with a grid of hidden tiles, trying to reveal all non-mine tiles while avoiding bombs.
>
## How to Play
### Objective
The goal is to reveal all non-mine tiles without clicking on a mine.
The number displayed on a revealed tile represents how many mines are adjacent to it.
If all non-mine tiles are revealed, the player wins the game.
Clicking on a mine results in an instant loss.
>
## Game Features & Functionalities
### 1. Revealing Tiles
Left-click on a tile to reveal it.
* If the tile contains a mine, the game ends with a loss.
* If the tile is empty and has no adjacent mines, the game will automatically reveal all connected empty tiles (recursive clearing).
* If the tile has adjacent mines, a number (1-8) appears, indicating the number of mines in the surrounding 8 tiles.
### 2. Flagging Mines
* Right-click on a tile to place a flag (if you suspect it contains a mine).
* Right-click again to remove the flag.
* Flags help keep track of suspected mines but do not automatically reveal tiles.
### 3. Game Controls
* Reset Button (Smiley Face 😊): Click this button to restart the game with a new randomized mine layout.
* Pause/Resume Button (⏸️ ▶️): Click to pause the game, which stops the timer and prevents tile interaction. Click again to resume.
* Debug Mode (⚙️): Toggles a mode where all mines are displayed, useful for testing/debugging.
* Leaderboard (📜): Opens a leaderboard window displaying the best five times recorded.
### 4. Win Condition
* The player wins when all non-mine tiles are revealed.
* The smiley face button changes to sunglasses 😎 upon victory.
* The leaderboard updates if the player’s time qualifies for the top 5.
### 5. Loss Condition
* Clicking on a mine results in an immediate loss.
* All mines are revealed, and the smiley face button changes to a sad face 😵.
* The player can reset the game and try again.
### 6. Leaderboard System
* The game keeps track of the top five fastest completion times.
* The leaderboard is stored in leaderboard.txt and updates when a new high score is achieved.
* The current session’s best time is marked with an asterisk (*).
* Only the top five scores are saved to the file (older/lower scores are removed).

## How to Run the Game
### Prerequisites
* You need SFML installed on your system.
* A C++ compiler that supports C++11 or later.
* CMake is recommended for project compilation.
### Steps to Build & Run
#### 1. Clone the Repository 
```
git clone https://github.com/timmyloos/minesweeper
cd minesweeper
```
#### 2. Compile the Code
```
mkdir build
cd build
cmake ..
make
```
#### 3. Run the Game
```
./minesweeper
```


