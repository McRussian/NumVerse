#pragma once

#include "abstract_game.h"

class TenMatchGame : public AbstractGame {
    Q_OBJECT
public:
    explicit TenMatchGame(std::string playerName, GameConfig config,
                          QObject* parent = nullptr);

    void selectCell(int row, int col) override;
    void append() override;

protected:
    std::unique_ptr<IGameRules> createRules() const override;
};
