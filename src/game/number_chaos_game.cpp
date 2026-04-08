#include "number_chaos_game.h"
#include "logic/rules/number_chaos_rules.h"

NumberChaosGame::NumberChaosGame(std::string playerName, GameConfig config, QObject* parent)
    : AbstractGame(std::move(playerName), std::move(config), parent)
{}

std::unique_ptr<IGameRules> NumberChaosGame::createRules() const
{
    return std::make_unique<NumberChaosRules>();
}
