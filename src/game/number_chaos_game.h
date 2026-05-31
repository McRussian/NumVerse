#pragma once

#include "abstract_game.h"

class NumberChaosGame : public AbstractGame {
    Q_OBJECT
public:
    explicit NumberChaosGame(std::string playerName, GameConfig config, QObject* parent = nullptr);

public:
    void selectCell(int row, int col) override;

protected:
    std::unique_ptr<IGameRules> createRules() const override;
};
