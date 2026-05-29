#pragma once

#include <QPainter>
#include <QRect>
#include <QString>
#include <functional>

struct GameDescriptor {
    int     id;
    QString name;
    QString description;
    std::function<void(QPainter&, const QRect&)> paintIcon;
};
