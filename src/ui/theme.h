#pragma once

#include <QColor>
#include <QString>

namespace Theme {

// Cell background colors
inline const QColor CellNormal     {220, 220, 220};
inline const QColor CellSelected   { 70, 130, 200};
inline const QColor CellHighlighted{255, 220,  80};
inline const QColor CellLocked     {160, 160, 160};
inline const QColor CellEmpty      {245, 245, 245};

// Text colors
inline const QColor TextNormal     { 30,  30,  30};
inline const QColor TextSelected   {255, 255, 255};
inline const QColor TextHighlighted{ 30,  30,  30};
inline const QColor TextLocked     {100, 100, 100};

// Grid
inline const QColor GridBackground{60, 60, 60};
inline constexpr int CellMinSize  = 60;
inline constexpr int CellSpacing  = 4;
inline constexpr int GridPadding  = 4;

// Default stylesheet — override by loading a .qss file at app level.
// Uses property selector: GameCellWidget[cellState="selected"] { ... }
inline QString defaultStyleSheet()
{
    return QString{
        "GridGameBoard {"
        "  background-color: %10;"
        "  border-radius: 6px;"
        "}"
        "GameCellWidget {"
        "  border: none;"
        "  border-radius: 4px;"
        "}"
        "GameCellWidget[cellState='normal']      { background-color: %1; color: %2; }"
        "GameCellWidget[cellState='selected']    { background-color: %3; color: %4; }"
        "GameCellWidget[cellState='highlighted'] { background-color: %5; color: %6; }"
        "GameCellWidget[cellState='locked']      { background-color: %7; color: %8; }"
        "GameCellWidget[cellState='empty']       { background-color: %9; color: %2; }"
    }
    .arg(CellNormal.name(),      TextNormal.name(),
         CellSelected.name(),    TextSelected.name(),
         CellHighlighted.name(), TextHighlighted.name(),
         CellLocked.name(),      TextLocked.name(),
         CellEmpty.name(),       GridBackground.name());
}

} // namespace Theme
