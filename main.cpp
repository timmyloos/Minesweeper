#include "WelcomeWindow.h"
#include "GameWindow.h"

using namespace std;

int main() {
    string playerName;

    // Initialize the welcome window
    WelcomeWindow welcomeWindow("files/font.ttf");
    if (welcomeWindow.run(playerName)) {
        // Initialize and run the game window
        GameWindow gameWindow(25, 16, 5, "files/font.ttf", "files/images", playerName);
        gameWindow.run();
    }

    return 0;
}