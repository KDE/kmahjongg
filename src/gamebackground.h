/* Copyright (C) 2019 Christian Krippendorf <Coding@Christian-Krippendorf.de>
 *
 * Kmahjongg is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free 
 * Software Foundation; either version 2 of the License, or (at your option) 
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51 
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA. */

#ifndef GAMEBACKGROUND_H
#define GAMEBACKGROUND_H

// Qt
#include <QGraphicsObject>
#include <QObject>

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
     * @param facePix The pixmap of the face
     */
    void setBackground(QPixmap *background);

    /**
     * Get the current background
     * @return Background image as pixmap
     */
    QPixmap getBackground() const;

    /**
     * Overrides the paint method of QGraphicsItem
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
            QWidget *widget) override;

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
    QPixmap *m_background;
};

#endif // GAMEBACKGROUND_H
