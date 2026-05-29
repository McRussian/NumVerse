#pragma once

#include <QPainter>
#include <QRect>
#include <QString>
#include <functional>
#include <string>
#include <cstdint>

#include "logic/data/game_config.h"

class AbstractGame;

// Возможности, специфичные для конкретной игры.
// MainWindow показывает/скрывает кнопки тулбара на основе этих флагов.
enum class GameFeature : uint32_t {
    None           = 0,
    ApplySelection = 1 << 0,  // подтверждение выбранной последовательности
};

inline GameFeature operator|(GameFeature a, GameFeature b) {
    return static_cast<GameFeature>(
        static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}
inline bool hasFeature(GameFeature flags, GameFeature f) {
    return (static_cast<uint32_t>(flags) & static_cast<uint32_t>(f)) != 0;
}

struct GameDescriptor {
    int          id;
    QString      name;
    QString      description;
    GameFeature  features = GameFeature::None;
    std::function<void(QPainter&, const QRect&)>          paintIcon;
    std::function<GameConfig(Difficulty)>                 makeConfig;
    std::function<AbstractGame*(std::string, GameConfig)> createGame;
};
