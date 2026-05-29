#pragma once

#include <QDialog>
#include <map>
#include "logic/score_board.h"

// Per-game score storage. Held by MainWindow, passed by reference to the dialog.
struct GameScoreData {
    int        id   = 0;
    QString    name;
    ScoreBoard byScore{RankingMode::ByScore};
    ScoreBoard byTime {RankingMode::ByTime};
};

class QComboBox;
class QTableWidget;

class HighScoreDialog : public QDialog {
    Q_OBJECT
public:
    explicit HighScoreDialog(std::map<int, GameScoreData>& scores,
                             QWidget* parent = nullptr);

private:
    struct Row {
        QString    gameName;
        uint8_t    level;
        GameResult result;
    };

    void   refresh();
    void   clearRecords();

    std::vector<Row> collectRows(int gameFilter, int levelFilter,
                                 RankingMode mode) const;

    static QString diffName(uint8_t level);
    static QString formatTime(uint32_t secs);

    std::map<int, GameScoreData>& m_scores;

    QComboBox*    m_gameFilter;
    QComboBox*    m_levelFilter;
    QComboBox*    m_sortMode;
    QTableWidget* m_table;
};
