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

// own
#include "gameremovedtiles.h"
#include "kmahjonggtileset.h"
#include "gamedata.h"

// Qt
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QTimer>

#include <klocalizedstring.h>


GameRemovedTiles::GameRemovedTiles(QGraphicsObject * object)
    : QGraphicsObject(object)
    , m_width(100)
    , m_height(100)
    , m_borderWidthFrac(0.05)
    , m_tileScale(0.9)
    , m_titleHeightFrac(0.1)
    , m_borderWidthPixel(0)
    , m_titleHeightPixel(0)
    , m_tileSpaceRow(0)
    , m_tileSpaceCol(0)
    , m_tileFaceWidth(0)
    , m_tileFaceHeight(0)
    , m_faceScale(1.0)
    , m_tileFaceWidthScaled(0)
    , m_tileFaceHeightScaled(0)
    , m_maxTilesRow(0)
    , m_maxTilesCol(0)
    , m_itemFaces(new QList<USHORT>())
    , m_tiles(nullptr)
    , m_gameData(nullptr)
{
}

GameRemovedTiles::~GameRemovedTiles()
{
    delete m_itemFaces;
}

void GameRemovedTiles::setSize(qreal width, qreal height)
{
    m_width = width;
    m_height = height;
}

void GameRemovedTiles::setTileset(KMahjonggTileset * tiles)
{
    m_tiles = tiles;
}

void GameRemovedTiles::prepareForGeometryChange()
{
    prepareGeometryChange();
}

void GameRemovedTiles::setGameData(GameData * gameData)
{
    m_gameData = gameData;
}

void GameRemovedTiles::updateTileCalculations()
{
    int maxTilesRow = 0;
    int maxTilesCol = 0;

    // Get the height and the width of the face tile. This has to be multiplied
    // by two, cause the value is related to half tile. (half positioning)
    m_tileFaceWidth = m_tiles->qWidth() * 2.0;
    m_tileFaceHeight = m_tiles->qHeight() * 2.0;
    m_tileFaceWidthScaled = m_tileFaceWidth * m_faceScale;
    m_tileFaceHeightScaled = m_tileFaceHeight * m_faceScale;

    m_borderWidthPixel = m_borderWidthFrac * m_width;
    m_titleHeightPixel = m_titleHeightFrac * m_height;

    maxTilesRow = static_cast<int>(
        (m_width - 2 * m_borderWidthPixel) / m_tileFaceWidthScaled
    );
    maxTilesCol = static_cast<int>(
        (m_height - 2 * m_borderWidthPixel - m_titleHeightPixel) /
        m_tileFaceHeightScaled
    );

    m_tileSpaceRow = ((m_width - 2 * m_borderWidthPixel) - 
        maxTilesRow * m_tileFaceWidthScaled) / (maxTilesRow - 1);
    m_tileSpaceCol = ((m_height - 
        2 * m_borderWidthPixel - m_titleHeightPixel) -
        maxTilesCol * m_tileFaceHeightScaled) / (maxTilesCol - 1);

    m_maxTilesRow = maxTilesRow;
    m_maxTilesCol = maxTilesCol;
}

void GameRemovedTiles::paint(QPainter *painter, const QStyleOptionGraphicsItem *, 
    QWidget *)
{
    updateTileCalculations();

    // General painter settings.
    painter->setRenderHint(QPainter::Antialiasing);

    // Paint the background.
    painter->setOpacity(0.5);
    QPainterPath path;
    path.addRoundedRect(QRectF(0, 0, m_width, m_height), 10, 10);
    painter->fillPath(path, Qt::black);

    // Paint the title text.
    painter->setPen(Qt::white);
    QFont font(painter->font());
    font.setPointSize(m_titleHeightPixel * 0.15);
    painter->setFont(font);
    painter->drawText(
        QRectF(m_borderWidthPixel, m_borderWidthPixel, m_width, m_titleHeightPixel),
        i18n("Removed tiles")
    );

    // Exit if no tileset has been set to this object.
    if (m_tiles == nullptr || m_itemFaces->isEmpty()) {
        return;
    }

    // Paint all the tiles.
    painter->setPen(QPen(Qt::white, 10));

    unsigned int row = 0;
    unsigned int col = 0;
    int start = m_itemFaces->size() - (m_maxTilesCol * m_maxTilesRow * 2);
    if (start < 0) {
        start *= 0;
    }
    for (int pos = start; pos < m_itemFaces->size() - 1; pos+=2) {
        if (col >= m_maxTilesRow) {
            row++;
            col = 0;
        }

        // Get the pixmap of the face.
        QPixmap face;
        face = m_tiles->tileface(m_itemFaces->at(pos));
        face = face.scaledToHeight(
            m_tileFaceHeightScaled, Qt::SmoothTransformation
        );

        // Paint the background of the face.
        QPainterPath pixPath;
        pixPath.addRoundedRect(
            QRectF(
                m_borderWidthPixel + col * m_tileSpaceRow + col * m_tileFaceWidth, 
                m_titleHeightPixel + row * m_tileSpaceCol + row * m_tileFaceHeight,
                m_tileFaceWidth, m_tileFaceHeight
            ), 10, 10
        );
        painter->setOpacity(1.0 - (m_itemFaces->size() - pos) / 100.0);
        painter->fillPath(pixPath, Qt::white);

        // Paint the pixmap of the face.
        painter->setOpacity(1.0 - (m_itemFaces->size() - pos) / 100.0);
        painter->drawPixmap(
            QPointF(
                m_borderWidthPixel + col * m_tileSpaceRow + col * m_tileFaceWidth,
                m_titleHeightPixel + row * m_tileSpaceCol + row * m_tileFaceHeight
            ), face
        );

        col++;
    }
}

void GameRemovedTiles::undo()
{
    if (m_itemFaces->size() >= 2) {
        m_itemFaces->removeLast();
        m_itemFaces->removeLast();
    }
}

void GameRemovedTiles::reset()
{
    m_itemFaces->clear();
}

QRectF GameRemovedTiles::boundingRect() const
{
    return QRectF(QPointF(0.0, 0.0), QSizeF(m_width, m_height));
}

QRectF GameRemovedTiles::rect() const
{
    return boundingRect();
}

void GameRemovedTiles::addItem(const POSITION & itemPos) 
{
    m_itemFaces->append(itemPos.f);
}

void GameRemovedTiles::removeLastItem()
{
    m_itemFaces->removeLast();
}
