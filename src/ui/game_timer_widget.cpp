#include "game_timer_widget.h"

#include <QLabel>
#include <QPainter>
#include <QStyleOption>
#include <QTimer>
#include <QVBoxLayout>

GameTimerWidget::GameTimerWidget(QWidget* parent)
    : QWidget(parent)
    , m_label(new QLabel("00:00", this))
    , m_timer(new QTimer(this))
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_label, 0, Qt::AlignCenter);
    setLayout(layout);

    m_label->setAlignment(Qt::AlignCenter);
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &GameTimerWidget::ticked);
}

void GameTimerWidget::start()
{
    m_timer->start();
}

void GameTimerWidget::stop()
{
    m_timer->stop();
}

void GameTimerWidget::reset()
{
    m_timer->stop();
    setTime(0);
}

void GameTimerWidget::setTime(uint32_t seconds)
{
    uint32_t m = seconds / 60;
    uint32_t s = seconds % 60;
    m_label->setText(QString("%1:%2")
        .arg(m, 2, 10, QChar('0'))
        .arg(s, 2, 10, QChar('0')));
}

void GameTimerWidget::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
