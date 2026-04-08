#include "game_panel.h"
#include "game_timer_widget.h"
#include "theme.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QStyleOption>

GamePanel::GamePanel(QWidget* parent)
    : QWidget(parent)
    , m_scoreLabel(new QLabel("Счёт: 0", this))
    , m_movesLabel(new QLabel(this))
    , m_timer(new GameTimerWidget(this))
    , m_hintBtn(new QPushButton("Подсказка", this))
    , m_surrenderBtn(new QPushButton("Сдаться", this))
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFixedHeight(Theme::PanelHeight);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 0, 12, 0);
    layout->setSpacing(16);

    layout->addWidget(m_scoreLabel);
    layout->addWidget(m_movesLabel);
    layout->addStretch();
    layout->addWidget(m_timer);
    layout->addStretch();
    layout->addWidget(m_hintBtn);
    layout->addWidget(m_surrenderBtn);

    setLayout(layout);

    connect(m_hintBtn,      &QPushButton::clicked, this, &GamePanel::hintClicked);
    connect(m_surrenderBtn, &QPushButton::clicked, this, &GamePanel::surrenderClicked);
}

void GamePanel::update(const GameState& state)
{
    m_scoreLabel->setText(QString("Счёт: %1").arg(state.score));

    if (state.movesLeft > 0)
        m_movesLabel->setText(QString("Ходы: %1").arg(state.movesLeft));
    else
        m_movesLabel->setText("Ходы: ∞");

    m_timer->setTime(state.secondsElapsed);
}

GameTimerWidget* GamePanel::timer() const
{
    return m_timer;
}

void GamePanel::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
