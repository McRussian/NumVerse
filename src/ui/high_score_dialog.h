#pragma once

#include <QDialog>
#include <map>
#include <vector>
#include "logic/score_board.h"

struct GameScoreData {
    int        id   = 0;
    QString    name;
    ScoreBoard byScore{RankingMode::ByScore};
    ScoreBoard byTime {RankingMode::ByTime};
};

class QComboBox;
class QTabWidget;
class QTableWidget;

class HighScoreDialog : public QDialog {
    Q_OBJECT
public:
    explicit HighScoreDialog(std::map<int, GameScoreData>& scores,
                             QWidget* parent = nullptr);

private:
    struct TabData {
        int           gameId;
        QComboBox*    levelFilter;
        QComboBox*    sortMode;
        QTableWidget* table;
    };

    void refreshTab(const TabData& tab);
    void clearCurrentTab();

    static QString diffName(uint8_t level);
    static QString formatTime(uint32_t secs);

    std::map<int, GameScoreData>& m_scores;
    QTabWidget*          m_tabs;
    std::vector<TabData> m_tabData;
};
