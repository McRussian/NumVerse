#pragma once

#include <QWidget>

class QLineEdit;

class MenuWidget : public QWidget {
    Q_OBJECT
public:
    explicit MenuWidget(QWidget* parent = nullptr);

    QString currentPlayerName() const;

signals:
    void gameSelected(int gameId);
    void playerChanged(const QString& name);
    void quitRequested();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QLineEdit* m_nameEdit;
};
