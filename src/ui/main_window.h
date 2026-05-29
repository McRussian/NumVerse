#pragma once

#include <QMainWindow>
#include "game_descriptor.h"
#include "logic/data/difficulty.h"

class QComboBox;
class QStackedWidget;
class MenuWidget;
class GameWindow;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

    Difficulty currentDifficulty() const;

private:
    QStackedWidget* m_stack;
    MenuWidget*     m_menu;
    GameWindow*     m_gameWindow = nullptr;
    QComboBox*      m_diffBox;

    void startGame(int gameId);
    void showMenu();
    void onPlayerChanged(const QString& name);
};
