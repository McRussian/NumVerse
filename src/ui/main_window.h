#pragma once

#include <QMainWindow>

class QStackedWidget;
class MenuWidget;
class GameWindow;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    QStackedWidget* m_stack;
    MenuWidget*     m_menu;
    GameWindow*     m_gameWindow = nullptr;

    void startGame(int gameId);
    void showMenu();
};
