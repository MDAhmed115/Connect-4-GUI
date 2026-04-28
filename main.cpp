#include "Game.h"
#include "Renderer.h"


int main() {
    Renderer renderer(800, 600, "Connect 4", 90);
    Game game;

    while (!renderer.shouldClose() && game.getState() != GameState::ExitGame) {
        game.update(GetFrameTime());
        renderer.draw(game);
    }

    return 0;
}
