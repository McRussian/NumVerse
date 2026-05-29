#pragma once

#include <QColor>
#include <QString>

namespace Theme {

// Cell background colors
inline const QColor CellNormal     {220, 220, 220};
inline const QColor CellSelected   { 70, 130, 200};
inline const QColor CellHighlighted{255, 220,  80};
inline const QColor CellHinted     {120, 200, 120};
inline const QColor CellLocked     {160, 160, 160};
inline const QColor CellEmpty      {245, 245, 245};

// Text colors
inline const QColor TextNormal     { 30,  30,  30};
inline const QColor TextSelected   {255, 255, 255};
inline const QColor TextHighlighted{ 30,  30,  30};
inline const QColor TextHinted     { 30,  30,  30};
inline const QColor TextLocked     {100, 100, 100};

// Panel
inline const QColor PanelBackground{45, 45, 45};
inline const QColor PanelText      {210, 210, 210};
inline constexpr int PanelHeight   = 56;

// Grid
inline const QColor GridBackground{60, 60, 60};
inline constexpr int CellMinSize  = 60;
inline constexpr int CellSpacing  = 4;
inline constexpr int GridPadding  = 4;

// Background for the main menu page
inline const QColor MenuBackground{30, 30, 40};

// Default stylesheet — override by loading a .qss file at app level.
// Uses property selector: GameCellWidget[cellState="selected"] { ... }
inline QString defaultStyleSheet()
{
    return QString{
        "MenuWidget {"
        "  background-color: %15;"
        "}"
        "QLabel#menuTitle {"
        "  color: #e8e8e8;"
        "}"
        "QLabel#menuSubtitle {"
        "  color: #aaaaaa;"
        "  font-size: 15px;"
        "}"
        "QPushButton#menuButton {"
        "  color: #e8e8e8;"
        "  font-size: 15px;"
        "  padding: 6px 20px;"
        "  border: 1px solid #555;"
        "  border-radius: 6px;"
        "  background-color: #3a3a50;"
        "}"
        "QPushButton#menuButton:hover  { background-color: #4a4a66; }"
        "QPushButton#menuButton:pressed { background-color: #2a2a3a; }"
        "QLineEdit#playerNameEdit {"
        "  background-color: #2a2a3a;"
        "  color: #e8e8e8;"
        "  border: 1px solid #555;"
        "  border-radius: 4px;"
        "  padding: 3px 8px;"
        "  font-size: 13px;"
        "}"
        "QPushButton#diffButton {"
        "  color: #a0a0b8;"
        "  font-size: 12px;"
        "  padding: 4px 14px;"
        "  border: 1px solid #444;"
        "  border-radius: 4px;"
        "  background-color: #2e2e42;"
        "}"
        "QPushButton#diffButton:hover   { background-color: #383855; color: #e0e0f0; }"
        "QPushButton#diffButton:checked {"
        "  background-color: #3a6ab5;"
        "  border-color: #5090e0;"
        "  color: #ffffff;"
        "}"
        "GridGameBoard {"
        "  background-color: %10;"
        "  border-radius: 6px;"
        "}"
        "GamePanel {"
        "  background-color: %11;"
        "}"
        "GamePanel QLabel {"
        "  color: %12;"
        "  font-size: 14px;"
        "}"
        "GamePanel QPushButton {"
        "  color: %12;"
        "  font-size: 13px;"
        "  padding: 4px 12px;"
        "  border: 1px solid #666;"
        "  border-radius: 4px;"
        "  background-color: #555;"
        "}"
        "GamePanel QPushButton:hover  { background-color: #666; }"
        "GamePanel QPushButton:pressed { background-color: #444; }"
        "GameCellWidget {"
        "  border: none;"
        "  border-radius: 4px;"
        "}"
        "GameCellWidget[cellState='normal']      { background-color: %1; color: %2; }"
        "GameCellWidget[cellState='selected']    { background-color: %3; color: %4; }"
        "GameCellWidget[cellState='highlighted'] { background-color: %5; color: %6; }"
        "GameCellWidget[cellState='hinted']      { background-color: %13; color: %14; }"
        "GameCellWidget[cellState='locked']      { background-color: %7; color: %8; }"
        "GameCellWidget[cellState='empty']       { background-color: %9; color: %2; }"
    }
    .arg(CellNormal.name(),      TextNormal.name(),
         CellSelected.name(),    TextSelected.name(),
         CellHighlighted.name(), TextHighlighted.name(),
         CellLocked.name(),      TextLocked.name(),
         CellEmpty.name())
    .arg(GridBackground.name())
    .arg(PanelBackground.name(), PanelText.name())
    .arg(CellHinted.name(), TextHinted.name())
    .arg(MenuBackground.name());
}

} // namespace Theme
