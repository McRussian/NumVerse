#include <QApplication>
#include "ui/game_window.h"
#include "ui/grid_game_board.h"
#include "ui/theme.h"
#include "game/number_chaos_game.h"
#include "logic/data/game_config.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setStyleSheet(Theme::defaultStyleSheet());

    GameConfig config;
    config.gridRows = 4;
    config.gridCols = 5;
    config.difficulty = Difficulty::Easy;

    auto* game  = new NumberChaosGame("Player", config);
    auto* board = new GridGameBoard();
    auto* window = new GameWindow(game, board);

    window->resize(600, 500);
    window->show();

    game->start();

    return app.exec();
}
