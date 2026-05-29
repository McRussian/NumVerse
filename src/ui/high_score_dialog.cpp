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
#include <QVBoxLayout>

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
    setMinimumSize(520, 380);

    // Filters
    m_gameFilter  = new QComboBox(this);
    m_levelFilter = new QComboBox(this);
    m_sortMode    = new QComboBox(this);

    m_gameFilter->addItem("Все игры", -1);
    for (const auto& [id, gsd] : m_scores)
        m_gameFilter->addItem(gsd.name, id);

    m_levelFilter->addItem("Все уровни", -1);
    for (int i = 0; i < 5; ++i)
        m_levelFilter->addItem(kDiffNames[i], i);

    m_sortMode->addItem("По очкам",  static_cast<int>(RankingMode::ByScore));
    m_sortMode->addItem("По времени", static_cast<int>(RankingMode::ByTime));

    auto* filterRow = new QHBoxLayout;
    filterRow->addWidget(new QLabel("Игра:", this));
    filterRow->addWidget(m_gameFilter);
    filterRow->addSpacing(12);
    filterRow->addWidget(new QLabel("Уровень:", this));
    filterRow->addWidget(m_levelFilter);
    filterRow->addSpacing(12);
    filterRow->addWidget(new QLabel("Сорт.:", this));
    filterRow->addWidget(m_sortMode);
    filterRow->addStretch();

    // Table
    m_table = new QTableWidget(0, 6, this);
    m_table->setHorizontalHeaderLabels(
        {"#", "Игра", "Игрок", "Очки", "Время", "Уровень"});
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_table->verticalHeader()->setVisible(false);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setAlternatingRowColors(true);

    auto* buttons  = new QDialogButtonBox(QDialogButtonBox::Close, this);
    auto* clearBtn = buttons->addButton("Очистить", QDialogButtonBox::ResetRole);
    connect(buttons,  &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(clearBtn, &QPushButton::clicked,       this, &HighScoreDialog::clearRecords);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(filterRow);
    layout->addWidget(m_table);
    layout->addWidget(buttons);

    auto refresh = [this] { this->refresh(); };
    connect(m_gameFilter,  &QComboBox::currentIndexChanged, this, refresh);
    connect(m_levelFilter, &QComboBox::currentIndexChanged, this, refresh);
    connect(m_sortMode,    &QComboBox::currentIndexChanged, this, refresh);

    refresh();
}

std::vector<HighScoreDialog::Row>
HighScoreDialog::collectRows(int gameFilter, int levelFilter, RankingMode mode) const
{
    std::vector<Row> rows;

    for (const auto& [id, gsd] : m_scores) {
        if (gameFilter >= 0 && id != gameFilter)
            continue;

        const ScoreBoard& board = (mode == RankingMode::ByScore)
            ? gsd.byScore : gsd.byTime;
        const auto& all = board.all();

        for (const auto& [lvl, vec] : all) {
            if (levelFilter >= 0 && static_cast<int>(lvl) != levelFilter)
                continue;
            for (const auto& r : vec)
                rows.push_back({gsd.name, lvl, r});
        }
    }

    // Sort combined results
    if (mode == RankingMode::ByScore) {
        std::sort(rows.begin(), rows.end(), [](const Row& a, const Row& b) {
            if (a.result.score != b.result.score)
                return a.result.score > b.result.score;
            return a.result.timeSecs < b.result.timeSecs;
        });
    } else {
        std::sort(rows.begin(), rows.end(), [](const Row& a, const Row& b) {
            if (a.result.timeSecs != b.result.timeSecs)
                return a.result.timeSecs < b.result.timeSecs;
            return a.result.score > b.result.score;
        });
    }

    return rows;
}

void HighScoreDialog::refresh()
{
    const int gameFilter  = m_gameFilter->currentData().toInt();
    const int levelFilter = m_levelFilter->currentData().toInt();
    const auto mode = static_cast<RankingMode>(m_sortMode->currentData().toInt());

    const auto rows = collectRows(gameFilter, levelFilter, mode);

    // Hide "Игра" column when a single game is selected
    const bool showGame = (gameFilter < 0);
    m_table->setColumnHidden(1, !showGame);

    m_table->setRowCount(static_cast<int>(rows.size()));

    auto cell = [](const QString& text,
                   Qt::Alignment align = Qt::AlignCenter) {
        auto* item = new QTableWidgetItem(text);
        item->setTextAlignment(align);
        return item;
    };

    for (int i = 0; i < static_cast<int>(rows.size()); ++i) {
        const auto& row = rows[i];
        m_table->setItem(i, 0, cell(QString::number(i + 1)));
        m_table->setItem(i, 1, cell(row.gameName,
                                    Qt::AlignLeft | Qt::AlignVCenter));
        m_table->setItem(i, 2, cell(QString::fromStdString(row.result.playerName),
                                    Qt::AlignLeft | Qt::AlignVCenter));
        m_table->setItem(i, 3, cell(QString::number(row.result.score)));
        m_table->setItem(i, 4, cell(formatTime(row.result.timeSecs)));
        m_table->setItem(i, 5, cell(diffName(row.level)));
    }
}

void HighScoreDialog::clearRecords()
{
    const int gameFilter  = m_gameFilter->currentData().toInt();
    const int levelFilter = m_levelFilter->currentData().toInt();

    QString what;
    if (gameFilter < 0 && levelFilter < 0)
        what = "все рекорды";
    else if (gameFilter < 0)
        what = QString("рекорды уровня «%1» во всех играх")
                   .arg(diffName(static_cast<uint8_t>(levelFilter)));
    else {
        const QString gname = m_scores.count(gameFilter)
            ? m_scores.at(gameFilter).name : QString::number(gameFilter);
        what = levelFilter < 0
            ? QString("все рекорды «%1»").arg(gname)
            : QString("рекорды «%1» уровня «%2»")
                  .arg(gname).arg(diffName(static_cast<uint8_t>(levelFilter)));
    }

    if (QMessageBox::question(this, "Очистить",
            QString("Удалить %1?").arg(what)) != QMessageBox::Yes)
        return;

    for (auto& [id, gsd] : m_scores) {
        if (gameFilter >= 0 && id != gameFilter)
            continue;
        if (levelFilter < 0) {
            gsd.byScore.clearAll();
            gsd.byTime.clearAll();
        } else {
            gsd.byScore.clear(static_cast<uint8_t>(levelFilter));
            gsd.byTime.clear(static_cast<uint8_t>(levelFilter));
        }
    }

    refresh();
}
