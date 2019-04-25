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

#ifndef GAMEREMOVEDTILES_H
#define GAMEREMOVEDTILES_H

// Qt
#include <QGraphicsObject>
#include <QObject>
#include <QList>

// KMahjongg
#include "kmtypes.h"

// Forward declarations
class KMahjonggTileset;
class GameData;


/**
 * A QGraphicsObject for representing the removed tiles of the current game.
 * @author Christian Krippendorf 
 */
class GameRemovedTiles : public QGraphicsObject
{
    Q_OBJECT

public:
    /**
     * Constructor
     * @param item The parent item
     */
    explicit GameRemovedTiles(QGraphicsObject *item = nullptr);
    ~GameRemovedTiles() override;

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

    /**
     * Add an removed tile.
     * @param itemPos POSITION object of the item.
     */
    void addItem(const POSITION & itemPos);

    /**
     * Set the game data object.
     * @param gameData the game data object.
     */
    void setGameData(GameData * gameData);

    /**
     * Remove a tile.
     */
    void removeLastItem();

    /**
     * Set the tileset for the tile pixmaps.
     * @param tiles KMahjonggTileset object.
     */
    void setTileset(KMahjonggTileset * tiles);

    /**
     * Calculate all values that neccessary to paint all tiles.
     */
    void updateTileCalculations();

private:
    qreal m_width;
    qreal m_height;
    qreal m_borderWidthFrac;
    qreal m_tileScale;
    qreal m_titleHeightFrac;
    qreal m_borderWidthPixel;
    qreal m_titleHeightPixel;
    qreal m_tileSpaceRow;
    qreal m_tileSpaceCol;
    qreal m_tileFaceWidth;
    qreal m_tileFaceHeight;
    qreal m_faceScale;
    qreal m_tileFaceWidthScaled;
    qreal m_tileFaceHeightScaled;
    unsigned int m_maxTilesRow;
    unsigned int m_maxTilesCol;

    QList<USHORT> * m_itemFaces;
    KMahjonggTileset * m_tiles;
    GameData * m_gameData;
};

#endif // GAMEREMOVEDTILES_H
