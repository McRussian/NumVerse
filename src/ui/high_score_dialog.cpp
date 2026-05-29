#include "high_score_dialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QTabWidget>
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

static QTableWidget* makeTable(QWidget* parent)
{
    auto* t = new QTableWidget(0, 5, parent);
    t->setHorizontalHeaderLabels({"#", "Игрок", "Очки", "Время", "Уровень"});
    t->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    t->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    t->verticalHeader()->setVisible(false);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setSelectionBehavior(QAbstractItemView::SelectRows);
    t->setAlternatingRowColors(true);
    return t;
}

HighScoreDialog::HighScoreDialog(ScoreBoard&    byScore,
                                 ScoreBoard&    byTime,
                                 const QString& gameName,
                                 QWidget*       parent)
    : QDialog(parent)
    , m_byScore(byScore)
    , m_byTime(byTime)
{
    setWindowTitle("Рекорды — " + gameName);
    setMinimumSize(480, 360);

    m_filter = new QComboBox(this);
    m_filter->addItem("Все уровни", -1);
    for (int i = 0; i < 5; ++i)
        m_filter->addItem(kDiffNames[i], i);

    auto* filterRow = new QHBoxLayout;
    filterRow->addWidget(new QLabel("Уровень:", this));
    filterRow->addWidget(m_filter);
    filterRow->addStretch();

    m_scoreTable = makeTable(this);
    m_timeTable  = makeTable(this);

    auto* tabs = new QTabWidget(this);
    tabs->addTab(m_scoreTable, "По очкам");
    tabs->addTab(m_timeTable,  "По времени");

    auto* buttons    = new QDialogButtonBox(QDialogButtonBox::Close, this);
    auto* clearBtn   = buttons->addButton("Очистить", QDialogButtonBox::ResetRole);
    connect(buttons,  &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(clearBtn, &QPushButton::clicked,       this, &HighScoreDialog::clearRecords);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(filterRow);
    layout->addWidget(tabs);
    layout->addWidget(buttons);

    connect(m_filter, &QComboBox::currentIndexChanged, this, [this](int idx) {
        refresh(m_filter->itemData(idx).toInt());
    });

    refresh(-1);
}

void HighScoreDialog::fillTable(QTableWidget*     table,
                                const ScoreBoard& board,
                                int               levelFilter)
{
    // Collect entries
    struct Row { uint8_t level; GameResult result; };
    std::vector<Row> rows;

    const auto& all = board.all();
    for (const auto& [lvl, vec] : all) {
        if (levelFilter >= 0 && static_cast<int>(lvl) != levelFilter)
            continue;
        for (const auto& r : vec)
            rows.push_back({lvl, r});
    }

    table->setRowCount(static_cast<int>(rows.size()));

    auto cell = [](const QString& text, Qt::Alignment align = Qt::AlignCenter) {
        auto* item = new QTableWidgetItem(text);
        item->setTextAlignment(align);
        return item;
    };

    for (int i = 0; i < static_cast<int>(rows.size()); ++i) {
        const auto& [lvl, r] = rows[i];
        table->setItem(i, 0, cell(QString::number(i + 1)));
        table->setItem(i, 1, cell(QString::fromStdString(r.playerName),
                                  Qt::AlignLeft | Qt::AlignVCenter));
        table->setItem(i, 2, cell(QString::number(r.score)));
        table->setItem(i, 3, cell(formatTime(r.timeSecs)));
        table->setItem(i, 4, cell(diffName(lvl)));
    }
}

void HighScoreDialog::clearRecords()
{
    int levelFilter = m_filter->currentData().toInt();
    QString what = levelFilter < 0
        ? "все рекорды"
        : QString("рекорды уровня «%1»").arg(diffName(static_cast<uint8_t>(levelFilter)));

    auto btn = QMessageBox::question(this, "Очистить",
                                     QString("Удалить %1?").arg(what));
    if (btn != QMessageBox::Yes)
        return;

    if (levelFilter < 0) {
        m_byScore.clearAll();
        m_byTime.clearAll();
    } else {
        m_byScore.clear(static_cast<uint8_t>(levelFilter));
        m_byTime.clear(static_cast<uint8_t>(levelFilter));
    }

    refresh(levelFilter);
}

void HighScoreDialog::refresh(int levelFilter)
{
    fillTable(m_scoreTable, m_byScore, levelFilter);
    fillTable(m_timeTable,  m_byTime,  levelFilter);
}
