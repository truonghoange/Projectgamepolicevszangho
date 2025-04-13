#include "Game.h"

int main(int argc, char* argv[]) {
    Game game;
    if (!game.Init("Police Game",600,600)) return -1;
    game.Run();
    game.Clean();
    return 0;
}