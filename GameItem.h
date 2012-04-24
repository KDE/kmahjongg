/* Copyright (C) 2012 Christian Krippendorf <Coding@Christian-Krippendorf.de>
 *
 * Kmahjongg is free software; you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA. */

#ifndef GAMEITEM_H
#define GAMEITEM_H

#include <QObject>
#include <QGraphicsItem>

#include "KmTypes.h"


/**
 * The tile of a mahjongg board.
 *
 * @author Christian Krippendorf */
class GameItem : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    /**
     * Constructor
     *
     * @param pItem The parent item
     * @param selected Should the item be selected */
    GameItem(bool selected, QGraphicsItem *pItem = 0);

    /**
     * Destructor */
    ~GameItem();

    /**
     * Get the actual angle.
     *
     * @return The angle that is actually set. */
    TileViewAngle getAngle() const;

    /**
     * Set the tile position.
     *
     * @param iX The x position.
     * @param iY The y position.
     * @param iZ The z position. */
    void setPosition(int iX, int iY, int iZ);

    /**
     * Get the positions.
     *
     * @return The position value. */
    int getXPosition() const;
    int getYPosition() const;
    int getZPosition() const;

    /**
     * Set the actual angle and therefore all pixmaps related to the angle.
     *
     * @param angle The angle of the item
     * @param pUnselPix The pixmap for a unselected item
     * @param pSelPix The pixmap for a selected item */
    void setAngle(TileViewAngle angle, QPixmap *pSelPix, QPixmap *pUnselPix);

    /**
     * Set the face of the stone.
     *
     * @param pFacePix The pixmap of the face. */
    void setFace(QPixmap *pFacePix);

    /**
     * Overrides the paint method of QGraphicsItem. */
    virtual void paint(QPainter *pPainter, const QStyleOptionGraphicsItem * pOption,
        QWidget *pWidget);

    /**
     * Overrides the boundingRect method of QGraphicsItem. */
    virtual QRectF boundingRect() const;

    /**
     * Returns the rect of the item.
     *
     * @return The rect of the item. */
    QRectF rect() const;

public slots:
    /**
     * Fade in the item. */
    void fadeIn();

    /**
     * Fade out the item. */
    void fadeOut();

private:
    /**
     * Updates the angle offset. Cause of 3D items, a shift related to the angle exist. */
    void updateFaceOffset();

    bool m_dying;
    int m_iZ;
    int m_iX;
    int m_iY;

    TileViewAngle m_angle;

    QPixmap *m_pUnselPix;
    QPixmap *m_pSelPix;
    QPixmap *m_pFacePix;

    QPointF m_faceOffset;
};


#endif // GAMEITEM_H
