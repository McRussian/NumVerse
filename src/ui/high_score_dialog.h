#pragma once

#include <QDialog>
#include "logic/score_board.h"

class QComboBox;
class QTableWidget;

class HighScoreDialog : public QDialog {
    Q_OBJECT
public:
    explicit HighScoreDialog(ScoreBoard&    byScore,
                             ScoreBoard&    byTime,
                             const QString& gameName,
                             QWidget*       parent = nullptr);

private:
    void refresh(int levelFilter);
    void fillTable(QTableWidget* table, const ScoreBoard& board, int levelFilter);
    void clearRecords();

    static QString diffName(uint8_t level);
    static QString formatTime(uint32_t secs);

    ScoreBoard& m_byScore;
    ScoreBoard& m_byTime;

    QComboBox*    m_filter;
    QTableWidget* m_scoreTable;
    QTableWidget* m_timeTable;
};
