#include "game_card_widget.h"

#include <QEnterEvent>
#include <QFont>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

GameCardWidget::GameCardWidget(const GameDescriptor& desc, QWidget* parent)
    : QWidget(parent)
    , m_desc(desc)
{
    setFixedSize(kCardW, kCardH);
    setCursor(Qt::PointingHandCursor);
}

void GameCardWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const QColor bgCard    = m_pressed ? QColor(45, 45, 62)
                           : m_hovered ? QColor(52, 52, 70)
                           :             QColor(40, 40, 55);
    const QColor bgIcon    = QColor(28, 28, 40);
    const QColor borderCol = m_hovered ? QColor(100, 140, 220) : QColor(65, 65, 85);

    // Card background
    QPainterPath card;
    card.addRoundedRect(rect(), 10, 10);
    p.fillPath(card, bgCard);
    p.setPen(QPen(borderCol, m_hovered ? 2 : 1));
    p.drawPath(card);

    // Icon area
    const QRect iconRect(1, 1, kCardW - 2, kIconH);
    QPainterPath iconPath;
    iconPath.addRoundedRect(iconRect, 9, 9);
    QPainterPath iconBottom;
    iconBottom.addRect(QRect(1, kIconH / 2, kCardW - 2, kIconH / 2));
    iconPath = iconPath.united(iconBottom);
    p.fillPath(iconPath, bgIcon);

    // Icon content
    const QRect iconInner = iconRect.adjusted(8, 8, -8, -8);
    m_desc.paintIcon(p, iconInner);

    // Game name
    QFont nameFont = p.font();
    nameFont.setPointSize(12);
    nameFont.setBold(true);
    p.setFont(nameFont);
    p.setPen(QColor(220, 220, 235));
    const QRect nameRect(8, kIconH + 8, kCardW - 16, 24);
    p.drawText(nameRect, Qt::AlignLeft | Qt::AlignVCenter, m_desc.name);

    // Description
    QFont descFont = p.font();
    descFont.setPointSize(9);
    descFont.setBold(false);
    p.setFont(descFont);
    p.setPen(QColor(140, 140, 160));
    const QRect descRect(8, kIconH + 34, kCardW - 16, kCardH - kIconH - 38);
    p.drawText(descRect, Qt::AlignLeft | Qt::TextWordWrap, m_desc.description);
}

void GameCardWidget::enterEvent(QEnterEvent* event)
{
    m_hovered = true;
    update();
    QWidget::enterEvent(event);
}

void GameCardWidget::leaveEvent(QEvent* event)
{
    m_hovered = false;
    m_pressed = false;
    update();
    QWidget::leaveEvent(event);
}

void GameCardWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_pressed = true;
        update();
    }
    QWidget::mousePressEvent(event);
}

void GameCardWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_pressed) {
        m_pressed = false;
        update();
        if (rect().contains(event->pos()))
            emit clicked(m_desc.id);
    }
    QWidget::mouseReleaseEvent(event);
}
