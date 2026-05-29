#include "abstract_game.h"

AbstractGame::AbstractGame(std::string playerName, GameConfig config, QObject* parent)
    : QObject(parent)
    , m_playerName(std::move(playerName))
    , m_config(std::move(config))
{}

void AbstractGame::start()
{
    m_logic.init(m_config, createRules());
    resetHints();
    emitStateSignals();
}

void AbstractGame::selectCell(int row, int col)
{
    m_logic.select(row, col);
    const GameState& s = m_logic.getState();
    emit boardChanged(s.board);
    emit stateChanged(s);
}

void AbstractGame::applySelection()
{
    m_logic.applySelection();
    resetHints();
    emitStateSignals();
}

void AbstractGame::tick(uint32_t secs)
{
    m_logic.tick(secs);
    emit stateChanged(m_logic.getState());
}

void AbstractGame::hint()
{
    if (m_hints.empty()) {
        m_hints = m_logic.getHint();
        m_hintIndex = 0;
    }
    if (m_hints.empty())
        return;
    emit hintReady(m_hints[m_hintIndex]);
    m_hintIndex = (m_hintIndex + 1) % m_hints.size();
}

void AbstractGame::undo()
{
    m_logic.undo();
    resetHints();
    const GameState& s = m_logic.getState();
    emit boardChanged(s.board);
    emit stateChanged(s);
}

void AbstractGame::shuffle()
{
    m_logic.shuffle();
    resetHints();
    const GameState& s = m_logic.getState();
    emit boardChanged(s.board);
    emit stateChanged(s);
}

void AbstractGame::surrender()
{
    m_logic.forfeit();
    emitStateSignals();
}

void AbstractGame::reset()
{
    m_logic.reset();
    resetHints();
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

void AbstractGame::resetHints()
{
    m_hints.clear();
    m_hintIndex = 0;
}

void AbstractGame::replaceBoardAndEmit(Board newBoard)
{
    m_logic.replaceBoard(std::move(newBoard));
    resetHints();
    const GameState& s = m_logic.getState();
    emit boardChanged(s.board);
    emit stateChanged(s);
}
