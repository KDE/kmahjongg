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
    , m_itemFaces(new QList<USHORT>())
    , m_tiles(nullptr)
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

void GameRemovedTiles::paint(QPainter *painter, const QStyleOptionGraphicsItem *, 
    QWidget *)
{
    int topSpace = 100; // in pixel
    int itemSpace = 10; // in pixel

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
    font.setPointSize(18);
    painter->setFont(font);
    painter->drawText(
        QRectF(10.0, 10.0, m_width, topSpace), i18n("Removed tiles")
    );

    // Exit if no tileset has been set to this object.
    if (nullptr == m_tiles || m_itemFaces->size() <= 0) {
        return;
    }

    // Paint all the tiles.
    painter->setPen(QPen(Qt::white, 10));

    // Calculate the number of tiles in one line. Therefore we need a face and
    // get the scaled width of it.
    QPixmap face;
    face = m_tiles->tileface(m_itemFaces->at(0));
    face = face.scaledToHeight(
        face.width() * 0.9, Qt::SmoothTransformation
    );
    int numTilesLine = (m_width - itemSpace) / (face.width() + itemSpace);

    for (int i = 0; i < m_itemFaces->size(); i++) {
        for (int j = 0; j < numTilesLine; j++) {
            if (i >= m_itemFaces->size()) {
                continue;
            }

            // Get the pixmap of the face.
            QPixmap face;
            face = m_tiles->tileface(m_itemFaces->at(i));
            face = face.scaledToHeight(
                face.width() * 0.9, Qt::SmoothTransformation
            );

            // Paint the background of the face.
            QPainterPath pixPath;
            pixPath.addRoundedRect(
                QRectF(
                    itemSpace + j * (itemSpace + face.width()), 
                    topSpace, face.width(),
                    face.height()
                ), 10, 10
            );
            painter->setOpacity(0.7);
            painter->fillPath(pixPath, Qt::white);

            // Paint the pixmap of the face.
            painter->setOpacity(1.0);
            painter->drawPixmap(
                QPointF(
                    itemSpace + j * (itemSpace + face.width()), topSpace
                ), face
            );

            // Only print every second item, cause they should be the same.
            i++; i++;
        }
    }
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
