#include "settings_dialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QVBoxLayout>

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Новая игра");
    setMinimumWidth(300);

    m_nameEdit = new QLineEdit("Player", this);

    m_difficultyBox = new QComboBox(this);
    m_difficultyBox->addItems({"Начинающий", "Лёгкий", "Средний", "Сложный", "Эксперт"});
    m_difficultyBox->setCurrentIndex(1);

    m_rowsSpin = new QSpinBox(this);
    m_rowsSpin->setRange(2, 8);
    m_rowsSpin->setValue(4);

    m_colsSpin = new QSpinBox(this);
    m_colsSpin->setRange(2, 10);
    m_colsSpin->setValue(5);

    auto* form = new QFormLayout;
    form->addRow("Имя игрока:", m_nameEdit);
    form->addRow("Сложность:", m_difficultyBox);
    form->addRow("Строки:", m_rowsSpin);
    form->addRow("Столбцы:", m_colsSpin);

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addSpacing(8);
    layout->addWidget(buttons);
}

PlayerSettings SettingsDialog::settings() const
{
    static const Difficulty kDiffMap[] = {
        Difficulty::Beginner, Difficulty::Easy,
        Difficulty::Medium,   Difficulty::Hard,
        Difficulty::Expert
    };

    GameConfig config;
    config.gridRows   = static_cast<uint8_t>(m_rowsSpin->value());
    config.gridCols   = static_cast<uint8_t>(m_colsSpin->value());
    config.difficulty = kDiffMap[m_difficultyBox->currentIndex()];

    return {m_nameEdit->text().toStdString(), config};
}
