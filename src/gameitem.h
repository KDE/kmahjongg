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

// Qt
#include <QGraphicsObject>

// KMahjongg
#include "kmtypes.h"

/**
 * The tile of a mahjongg board.
 *
 * @author Christian Krippendorf */
class GameItem : public QGraphicsObject
{
    Q_OBJECT

public:
    /**
     * Constructor
     *
     * @param selected Should the item be selected
     * @param item The parent item */
    explicit GameItem(bool selected, QGraphicsObject * item = nullptr);
    ~GameItem() override;

    /**
     * Get the actual angle.
     *
     * @return The angle that is actually set. */
    TileViewAngle getAngle() const;

    /**
     * Set the items grid position.
     *
     * @param x The x position.
     * @param y The y position.
     * @param z The z position. */
    void setGridPos(USHORT x, USHORT y, USHORT z);

    /**
     * Set the item grid position with the POSITION struct.
     *
     * @param stPos The POSITION type. */
    void setGridPos(POSITION & stPos);

    /**
     * Set the face id of the pixmap.
     *
     * @param faceId The face id to set up. */
    void setFaceId(USHORT faceId);

    /**
     * Get the face id.
     *
     * @return The face id.*/
    USHORT getFaceId() const;

    /**
     * Get the POSITION struct that is actually set.
     *
     * @return The POSITION struct. */
    POSITION getGridPos() const;

    /**
     * Get the grid positions.
     *
     * @return The position value. */
    USHORT getGridPosX() const;
    USHORT getGridPosY() const;
    USHORT getGridPosZ() const;

    /**
     * Set the actual angle and therefore all pixmaps related to the angle.
     *
     * @param angle The angle of the item
     * @param unselPix The pixmap for a unselected item
     * @param selPix The pixmap for a selected item
     * @param shadowWidth The width of the shadow
     * @param shadowHeight The height of the shadow */
    void setAngle(TileViewAngle angle, QPixmap * selPix, QPixmap * unselPix, int shadowWidth,
                  int shadowHeight);

    /**
     * Set the face of the stone.
     *
     * @param facePix The pixmap of the face. */
    void setFace(QPixmap * facePix);

    /**
     * Overrides the paint method of QGraphicsItem. */
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
                       QWidget * widget) override;

    /**
     * Overrides the boundingRect method of QGraphicsItem. */
    QRectF boundingRect() const override;

    /**
     * Returns the rect of the item.
     *
     * @return The rect of the item. */
    QRectF rect() const;

    /**
     * Test whether the point is on the shadow or not.
     *
     * @param position The position where the point is. */
    bool isShadow(QPointF const position) const;

    /**
     * */
    int getShadowDeltaX() const;

    /**
     * */
    int getShadowDeltaY() const;

    /**
     * Called in GameView::resizeTileset() before reloading the tiles.
     */
    void prepareForGeometryChange();

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
    int m_shadowWidth;
    int m_shadowHeight;

    POSITION m_stPos;

    TileViewAngle m_angle;

    QPixmap * m_selPix;
    QPixmap * m_unselPix;
    QPixmap * m_facePix;

    QPointF m_faceOffset;
};

#endif // GAMEITEM_H
