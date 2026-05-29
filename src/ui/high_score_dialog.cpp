#include "high_score_dialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <algorithm>

static const char* kDiffNames[] = {
    "Новичок", "Лёгкий", "Средний", "Сложный", "Эксперт"
};

QString HighScoreDialog::diffName(uint8_t level)
{
    return level < 5 ? kDiffNames[level] : QString::number(level);
}

QString HighScoreDialog::formatTime(uint32_t secs)
{
    return QString("%1:%2")
        .arg(secs / 60, 2, 10, QChar('0'))
        .arg(secs % 60, 2, 10, QChar('0'));
}

HighScoreDialog::HighScoreDialog(std::map<int, GameScoreData>& scores,
                                 QWidget* parent)
    : QDialog(parent)
    , m_scores(scores)
{
    setWindowTitle("Таблица рекордов");
    setMinimumSize(500, 380);

    m_tabs = new QTabWidget(this);

    for (auto& [id, gsd] : m_scores) {
        auto* page = new QWidget;

        auto* levelFilter = new QComboBox(page);
        auto* sortMode    = new QComboBox(page);

        levelFilter->addItem("Все уровни", -1);
        for (int i = 0; i < 5; ++i)
            levelFilter->addItem(kDiffNames[i], i);

        sortMode->addItem("По очкам",   static_cast<int>(RankingMode::ByScore));
        sortMode->addItem("По времени", static_cast<int>(RankingMode::ByTime));

        auto* filterRow = new QHBoxLayout;
        filterRow->addWidget(new QLabel("Уровень:"));
        filterRow->addWidget(levelFilter);
        filterRow->addSpacing(12);
        filterRow->addWidget(new QLabel("Сортировка:"));
        filterRow->addWidget(sortMode);
        filterRow->addStretch();

        auto* table = new QTableWidget(0, 5, page);
        table->setHorizontalHeaderLabels({"#", "Игрок", "Очки", "Время", "Уровень"});
        table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        table->verticalHeader()->setVisible(false);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setAlternatingRowColors(true);

        auto* layout = new QVBoxLayout(page);
        layout->addLayout(filterRow);
        layout->addWidget(table);

        m_tabs->addTab(page, gsd.name);

        TabData td{id, levelFilter, sortMode, table};
        m_tabData.push_back(td);

        auto refresh = [this, td] { refreshTab(td); };
        connect(levelFilter, &QComboBox::currentIndexChanged, this, refresh);
        connect(sortMode,    &QComboBox::currentIndexChanged, this, refresh);

        refreshTab(td);
    }

    auto* buttons  = new QDialogButtonBox(QDialogButtonBox::Close, this);
    auto* clearBtn = buttons->addButton("Очистить", QDialogButtonBox::ResetRole);
    connect(buttons,  &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(clearBtn, &QPushButton::clicked,       this, &HighScoreDialog::clearCurrentTab);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(m_tabs);
    layout->addWidget(buttons);
}

void HighScoreDialog::refreshTab(const TabData& tab)
{
    if (!m_scores.count(tab.gameId)) return;

    const int levelFilter = tab.levelFilter->currentData().toInt();
    const auto mode = static_cast<RankingMode>(tab.sortMode->currentData().toInt());
    const GameScoreData& gsd = m_scores.at(tab.gameId);
    const ScoreBoard& board  = (mode == RankingMode::ByScore)
        ? gsd.byScore : gsd.byTime;

    struct Row { uint8_t level; GameResult result; };
    std::vector<Row> rows;

    for (const auto& [lvl, vec] : board.all()) {
        if (levelFilter >= 0 && static_cast<int>(lvl) != levelFilter)
            continue;
        for (const auto& r : vec)
            rows.push_back({lvl, r});
    }

    if (mode == RankingMode::ByScore) {
        std::sort(rows.begin(), rows.end(), [](const Row& a, const Row& b) {
            return a.result.score != b.result.score
                ? a.result.score > b.result.score
                : a.result.timeSecs < b.result.timeSecs;
        });
    } else {
        std::sort(rows.begin(), rows.end(), [](const Row& a, const Row& b) {
            return a.result.timeSecs != b.result.timeSecs
                ? a.result.timeSecs < b.result.timeSecs
                : a.result.score > b.result.score;
        });
    }

    tab.table->setRowCount(static_cast<int>(rows.size()));

    auto cell = [](const QString& text,
                   Qt::Alignment align = Qt::AlignCenter) {
        auto* item = new QTableWidgetItem(text);
        item->setTextAlignment(align);
        return item;
    };

    for (int i = 0; i < static_cast<int>(rows.size()); ++i) {
        const auto& row = rows[i];
        tab.table->setItem(i, 0, cell(QString::number(i + 1)));
        tab.table->setItem(i, 1, cell(QString::fromStdString(row.result.playerName),
                                      Qt::AlignLeft | Qt::AlignVCenter));
        tab.table->setItem(i, 2, cell(QString::number(row.result.score)));
        tab.table->setItem(i, 3, cell(formatTime(row.result.timeSecs)));
        tab.table->setItem(i, 4, cell(diffName(row.level)));
    }
}

void HighScoreDialog::clearCurrentTab()
{
    const int tabIdx = m_tabs->currentIndex();
    if (tabIdx < 0 || tabIdx >= static_cast<int>(m_tabData.size())) return;

    const TabData& tab = m_tabData[tabIdx];
    if (!m_scores.count(tab.gameId)) return;

    const int levelFilter = tab.levelFilter->currentData().toInt();
    GameScoreData& gsd = m_scores.at(tab.gameId);
    const QString gname = gsd.name;

    QString what = levelFilter < 0
        ? QString("все рекорды «%1»").arg(gname)
        : QString("рекорды «%1» уровня «%2»")
              .arg(gname).arg(diffName(static_cast<uint8_t>(levelFilter)));

    if (QMessageBox::question(this, "Очистить",
            QString("Удалить %1?").arg(what)) != QMessageBox::Yes)
        return;

    if (levelFilter < 0) {
        gsd.byScore.clearAll();
        gsd.byTime.clearAll();
    } else {
        gsd.byScore.clear(static_cast<uint8_t>(levelFilter));
        gsd.byTime.clear(static_cast<uint8_t>(levelFilter));
    }

    refreshTab(tab);
}
