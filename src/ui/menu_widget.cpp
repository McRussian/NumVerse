#include "menu_widget.h"
#include "game_card_widget.h"
#include "game_catalog.h"

#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QStyleOption>
#include <QVBoxLayout>

MenuWidget::MenuWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* title = new QLabel("NumVerse", this);
    title->setObjectName("menuTitle");
    title->setAlignment(Qt::AlignCenter);
    QFont f = title->font();
    f.setPointSize(36);
    f.setBold(true);
    title->setFont(f);

    auto* subtitle = new QLabel("Выбери игру", this);
    subtitle->setObjectName("menuSubtitle");
    subtitle->setAlignment(Qt::AlignCenter);

    // Cards row
    auto* cardsWidget = new QWidget(this);
    auto* cardsLayout = new QHBoxLayout(cardsWidget);
    cardsLayout->setAlignment(Qt::AlignCenter);
    cardsLayout->setSpacing(20);
    cardsLayout->setContentsMargins(0, 0, 0, 0);

    for (const auto& desc : GameCatalog::allGames()) {
        auto* card = new GameCardWidget(desc, cardsWidget);
        cardsLayout->addWidget(card);
        connect(card, &GameCardWidget::clicked, this, &MenuWidget::gameSelected);
    }

    auto* quitBtn = new QPushButton("Выход", this);
    quitBtn->setObjectName("menuButton");
    quitBtn->setFixedHeight(36);
    quitBtn->setFixedWidth(120);

    auto* quitRow = new QHBoxLayout;
    quitRow->addStretch();
    quitRow->addWidget(quitBtn);
    quitRow->addStretch();

    auto* layout = new QVBoxLayout(this);
    layout->addStretch(1);
    layout->addWidget(title);
    layout->addSpacing(4);
    layout->addWidget(subtitle);
    layout->addStretch(1);
    layout->addWidget(cardsWidget);
    layout->addStretch(1);
    layout->addLayout(quitRow);
    layout->addSpacing(20);
    layout->setContentsMargins(40, 30, 40, 20);

    connect(quitBtn, &QPushButton::clicked, this, &MenuWidget::quitRequested);
}

void MenuWidget::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(event);
}
