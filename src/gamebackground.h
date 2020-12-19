/*
    SPDX-FileCopyrightText: 2019 Christian Krippendorf <Coding@Christian-Krippendorf.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef GAMEBACKGROUND_H
#define GAMEBACKGROUND_H

// Qt
#include <QGraphicsObject>
#include <QBrush>

// KMahjongg
#include "kmtypes.h"


/**
 * The background item of the kmahjongg board.
 * @author Christian Krippendorf 
 */
class GameBackground : public QGraphicsObject
{
    Q_OBJECT

public:
    /**
     * Constructor
     * @param item The parent item
     */
    explicit GameBackground(QGraphicsObject *item = nullptr);
    ~GameBackground() override;

    /**
     * Set the background
     * @param background The pixmap of the background
     */
    void setBackground(const QBrush & background);

    /**
     * Overrides the paint method of QGraphicsItem
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
            QWidget *widget) override;

    /**
     * Set size of element
     * @param width Width of element in pixels
     * @param height Height of element in pixels
     */
    void setSize(qreal width, qreal height);
    
    /**
     * Overrides the boundingRect method of QGraphicsItem
     */
    QRectF boundingRect() const override;

    /**
     * Returns the rect of the item
     * @return The rect of the item
     */
    QRectF rect() const;

    /**
     * Called in GameView::resizeTileset() before reloading the tiles
     */
    void prepareForGeometryChange();

private:
    QBrush m_background;

    qreal m_width;
    qreal m_height;
};

#endif // GAMEBACKGROUND_H
