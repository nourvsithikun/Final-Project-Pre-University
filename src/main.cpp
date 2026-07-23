#include "Game.h"

#include <exception>
#include <iostream>

int main() {
    try {
        Game game;
        game.Run();
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Fatal error: " << error.what() << '\n';
        return 1;
    }
}

