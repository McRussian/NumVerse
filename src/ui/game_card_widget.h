#pragma once

#include <QWidget>
#include "game_descriptor.h"

class GameCardWidget : public QWidget {
    Q_OBJECT
public:
    explicit GameCardWidget(const GameDescriptor& desc, QWidget* parent = nullptr);

signals:
    void clicked(int gameId);

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    static constexpr int kIconH  = 140;
    static constexpr int kCardW  = 190;
    static constexpr int kCardH  = 210;

    GameDescriptor m_desc;
    bool           m_hovered = false;
    bool           m_pressed = false;
};
