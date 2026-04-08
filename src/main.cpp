#include <QApplication>
#include "ui/grid_game_board.h"
#include "ui/theme.h"
#include "logic/data/board.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setStyleSheet(Theme::defaultStyleSheet());

    // Тестовая доска — все состояния ячеек
    Board board(4, 5);
    board.at(0, 0).setValue(1);
    board.at(0, 1).setValue(2);  board.at(0, 1).setState(CellState::Selected);
    board.at(0, 2).setValue(3);  board.at(0, 2).setState(CellState::Highlighted);
    board.at(0, 3).setValue(4);  board.at(0, 3).setState(CellState::Locked);
    board.at(0, 4).setState(CellState::Empty);

    board.at(1, 0).setValue(5);
    board.at(1, 1).setValue(8);
    board.at(1, 2).setValue(13); board.at(1, 2).setState(CellState::Selected);
    board.at(1, 3).setValue(21);
    board.at(1, 4).setValue(34);

    board.at(2, 0).setValue(2);  board.at(2, 0).setState(CellState::Highlighted);
    board.at(2, 1).setValue(4);
    board.at(2, 2).setValue(6);
    board.at(2, 3).setValue(8);  board.at(2, 3).setState(CellState::Selected);
    board.at(2, 4).setValue(10);

    board.at(3, 0).setValue(100);
    board.at(3, 1).setValue(200); board.at(3, 1).setState(CellState::Locked);
    board.at(3, 2).setValue(300);
    board.at(3, 3).setState(CellState::Empty);
    board.at(3, 4).setValue(400);

    auto* boardWidget = new GridGameBoard();
    boardWidget->resize(500, 400);
    boardWidget->updateBoard(board);
    boardWidget->show();

    return app.exec();
}
