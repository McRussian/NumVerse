#pragma once

#include <QDialog>
#include "logic/data/game_config.h"
#include <string>

class QLineEdit;
class QComboBox;
class QSpinBox;

struct PlayerSettings {
    std::string playerName;
    GameConfig  config;
};

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget* parent = nullptr);

    PlayerSettings settings() const;

private:
    QLineEdit* m_nameEdit;
    QComboBox* m_difficultyBox;
    QSpinBox*  m_rowsSpin;
    QSpinBox*  m_colsSpin;
};
