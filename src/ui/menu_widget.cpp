#include "menu_widget.h"
#include "game_card_widget.h"
#include "game_catalog.h"

#include <QButtonGroup>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPushButton>
#include <QSettings>
#include <QStyleOption>
#include <QVBoxLayout>

static const char* kDiffLabels[] = {"Новичок", "Лёгкий", "Средний", "Сложный", "Эксперт"};

MenuWidget::MenuWidget(QWidget* parent)
    : QWidget(parent)
{
    // Title
    auto* title = new QLabel("NumVerse", this);
    title->setObjectName("menuTitle");
    title->setAlignment(Qt::AlignCenter);
    QFont f = title->font();
    f.setPointSize(36);
    f.setBold(true);
    title->setFont(f);

    // Player name row
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

    // Difficulty buttons
    auto* diffRow = new QHBoxLayout;
    diffRow->addStretch();
    m_diffGroup = new QButtonGroup(this);
    for (int i = 0; i < 5; ++i) {
        auto* btn = new QPushButton(kDiffLabels[i], this);
        btn->setObjectName("diffButton");
        btn->setCheckable(true);
        btn->setFixedHeight(30);
        m_diffGroup->addButton(btn, i);
        diffRow->addWidget(btn);
    }
    diffRow->addStretch();

    // Game cards
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

    // Quit button
    auto* quitBtn = new QPushButton("Выход", this);
    quitBtn->setObjectName("menuButton");
    quitBtn->setFixedHeight(36);
    quitBtn->setFixedWidth(120);
    auto* quitRow = new QHBoxLayout;
    quitRow->addStretch();
    quitRow->addWidget(quitBtn);
    quitRow->addStretch();

    // Main layout
    auto* layout = new QVBoxLayout(this);
    layout->addStretch(1);
    layout->addWidget(title);
    layout->addSpacing(16);
    layout->addLayout(playerRow);
    layout->addSpacing(10);
    layout->addLayout(diffRow);
    layout->addStretch(1);
    layout->addWidget(cardsWidget);
    layout->addStretch(1);
    layout->addLayout(quitRow);
    layout->addSpacing(20);
    layout->setContentsMargins(40, 30, 40, 20);

    connect(quitBtn, &QPushButton::clicked, this, &MenuWidget::quitRequested);

    connect(m_nameEdit, &QLineEdit::textChanged, this, [this](const QString& name) {
        QSettings().setValue("lastPlayer", name.isEmpty() ? "Player" : name);
        loadPlayerSettings(name);
    });

    connect(m_diffGroup, &QButtonGroup::idClicked, this, [this](int) {
        savePlayerDifficulty();
    });

    // Load last player
    QSettings s;
    QString lastName = s.value("lastPlayer", "Player").toString();
    m_nameEdit->setText(lastName);
    loadPlayerSettings(lastName);
}

QString MenuWidget::currentPlayerName() const
{
    QString n = m_nameEdit->text().trimmed();
    return n.isEmpty() ? "Player" : n;
}

Difficulty MenuWidget::currentDifficulty() const
{
    return static_cast<Difficulty>(m_diffGroup->checkedId());
}

void MenuWidget::loadPlayerSettings(const QString& name)
{
    QString key = "players/" + (name.isEmpty() ? "Player" : name) + "/difficulty";
    int idx = QSettings().value(key, 1).toInt(); // default Easy
    if (auto* btn = m_diffGroup->button(idx))
        btn->setChecked(true);
}

void MenuWidget::savePlayerDifficulty()
{
    QString name = currentPlayerName();
    QSettings().setValue("players/" + name + "/difficulty", m_diffGroup->checkedId());
}

void MenuWidget::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(event);
}
