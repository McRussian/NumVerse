#pragma once

#include <QWidget>

class QLabel;
class QTimer;

class GameTimerWidget : public QWidget {
    Q_OBJECT
public:
    explicit GameTimerWidget(QWidget* parent = nullptr);

    void start();
    void stop();
    void reset();

    void setTime(uint32_t seconds);

signals:
    void ticked();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QLabel* m_label;
    QTimer* m_timer;
};
