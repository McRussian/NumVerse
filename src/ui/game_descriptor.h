#pragma once

#include <QPainter>
#include <QRect>
#include <QString>
#include <functional>
#include <string>

#include "logic/data/game_config.h"

class AbstractGame;

struct GameDescriptor {
    int     id;
    QString name;
    QString description;
    std::function<void(QPainter&, const QRect&)>          paintIcon;
    std::function<GameConfig(Difficulty)>                 makeConfig;
    std::function<AbstractGame*(std::string, GameConfig)> createGame;
};
