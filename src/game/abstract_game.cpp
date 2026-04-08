#include "abstract_game.h"

AbstractGame::AbstractGame(std::string playerName, GameConfig config, QObject* parent)
    : QObject(parent)
    , m_playerName(std::move(playerName))
    , m_config(std::move(config))
{}

void AbstractGame::start()
{
    m_logic.init(m_config, createRules());
    emitStateSignals();
}

void AbstractGame::selectCell(int row, int col)
{
    m_logic.select(row, col);
    emit stateChanged(m_logic.getState());
}

void AbstractGame::applySelection()
{
    m_logic.applySelection();
    emitStateSignals();
}

void AbstractGame::tick(uint32_t secs)
{
    m_logic.tick(secs);
    emit stateChanged(m_logic.getState());
}

void AbstractGame::hint()
{
    Selection h = m_logic.getHint();
    if (!h.empty())
        emit hintReady(h);
}

void AbstractGame::surrender()
{
    m_logic.forfeit();
    emitStateSignals();
}

void AbstractGame::reset()
{
    m_logic.reset();
    emitStateSignals();
}

const GameState& AbstractGame::state() const
{
    return m_logic.getState();
}

void AbstractGame::emitStateSignals()
{
    const GameState& s = m_logic.getState();
    emit boardChanged(s.board);
    emit stateChanged(s);

    if (s.status == GameStatus::Won || s.status == GameStatus::Lost)
        emit gameOver(m_logic.buildResult(m_playerName));
}
