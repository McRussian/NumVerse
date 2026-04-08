#pragma once

#include <QWidget>
#include "logic/data/game_state.h"

class GameTimerWidget;
class QLabel;
class QPushButton;

class GamePanel : public QWidget {
    Q_OBJECT
public:
    explicit GamePanel(QWidget* parent = nullptr);

    void update(const GameState& state);

    GameTimerWidget* timer() const;

signals:
    void hintClicked();
    void surrenderClicked();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QLabel*          m_scoreLabel;
    QLabel*          m_movesLabel;
    GameTimerWidget* m_timer;
    QPushButton*     m_hintBtn;
    QPushButton*     m_surrenderBtn;
};
