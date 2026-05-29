#pragma once

#include <QWidget>
#include "logic/data/difficulty.h"

class QButtonGroup;
class QLineEdit;

class MenuWidget : public QWidget {
    Q_OBJECT
public:
    explicit MenuWidget(QWidget* parent = nullptr);

    QString    currentPlayerName() const;
    Difficulty currentDifficulty() const;

signals:
    void gameSelected(int gameId);
    void quitRequested();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QLineEdit*    m_nameEdit;
    QButtonGroup* m_diffGroup;

    void loadPlayerSettings(const QString& name);
    void savePlayerDifficulty();
};
