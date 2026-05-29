#include "menu_widget.h"
#include "game_card_widget.h"
#include "game_catalog.h"

#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPushButton>
#include <QSettings>
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

    auto* playerLabel = new QLabel("Игрок:", this);
    playerLabel->setObjectName("menuSubtitle");
    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setObjectName("playerNameEdit");
    m_nameEdit->setFixedWidth(180);
    m_nameEdit->setPlaceholderText("Ваше имя");

    auto* playerRow = new QHBoxLayout;
    playerRow->addStretch();
    playerRow->addWidget(playerLabel);
    playerRow->addSpacing(8);
    playerRow->addWidget(m_nameEdit);
    playerRow->addStretch();

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

    auto* recordsBtn = new QPushButton("Рекорды", this);
    auto* quitBtn    = new QPushButton("Выход",   this);
    recordsBtn->setObjectName("menuButton");
    quitBtn->setObjectName("menuButton");
    recordsBtn->setFixedHeight(36);
    quitBtn->setFixedHeight(36);
    recordsBtn->setFixedWidth(120);
    quitBtn->setFixedWidth(120);
    auto* quitRow = new QHBoxLayout;
    quitRow->addStretch();
    quitRow->addWidget(recordsBtn);
    quitRow->addSpacing(12);
    quitRow->addWidget(quitBtn);
    quitRow->addStretch();

    auto* layout = new QVBoxLayout(this);
    layout->addStretch(1);
    layout->addWidget(title);
    layout->addSpacing(16);
    layout->addLayout(playerRow);
    layout->addStretch(1);
    layout->addWidget(cardsWidget);
    layout->addStretch(1);
    layout->addLayout(quitRow);
    layout->addSpacing(20);
    layout->setContentsMargins(40, 30, 40, 20);

    connect(recordsBtn, &QPushButton::clicked, this, &MenuWidget::recordsRequested);
    connect(quitBtn,    &QPushButton::clicked, this, &MenuWidget::quitRequested);

    // Set initial value without triggering the signal
    QSettings s;
    m_nameEdit->setText(s.value("lastPlayer", "Player").toString());

    connect(m_nameEdit, &QLineEdit::textChanged, this, [this](const QString& name) {
        const QString effective = name.trimmed().isEmpty() ? "Player" : name.trimmed();
        QSettings().setValue("lastPlayer", effective);
        emit playerChanged(effective);
    });
}

QString MenuWidget::currentPlayerName() const
{
    const QString n = m_nameEdit->text().trimmed();
    return n.isEmpty() ? "Player" : n;
}

void MenuWidget::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(event);
}
