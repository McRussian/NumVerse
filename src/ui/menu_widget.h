#pragma once

#include <QWidget>

class MenuWidget : public QWidget {
    Q_OBJECT
public:
    explicit MenuWidget(QWidget* parent = nullptr);

signals:
    void gameSelected(int gameId);
    void quitRequested();

protected:
    void paintEvent(QPaintEvent* event) override;
};
