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

#include "GameWidget.h"
#include "GameData.h"
#include "GameScene.h"
#include "GameItem.h"
#include "kmahjongglayout.h"
#include "kmahjonggtileset.h"
#include "kmahjonggbackground.h"
#include "prefs.h"

#include <KLocale>
#include <KRandom>
#include <KDebug>

#include <QResizeEvent>


GameWidget::GameWidget(QGraphicsScene *pScene, QWidget *pParent)
    : QGraphicsView(pScene, pParent),
    m_pGameData(0)
{
    m_angle = (TileViewAngle) Prefs::angle();

    // Create tiles...
    m_pTiles = new KMahjonggTileset();

    // Load background
    m_pBackground = new KMahjonggBackground();

    //~ updateWidget(true);

    // Connect to the scene...
    connect(pScene, SIGNAL(backgroundChanged(QString const &rBackgroundPath)), this,
        SLOT(setBackgroundPath(QString const &rBackgroundPath)));
    connect(pScene, SIGNAL(tilesetChanged(QString const &rTilesetPath)), this,
        SLOT(setTilesetPath(QString const &rTilesetPath)));
}

GameWidget::~GameWidget()
{
    delete m_pBackground;
    delete m_pTiles;
}

bool GameWidget::setTilesetPath(QString const &rTilesetPath)
{
    if (m_pTiles->loadTileset(rTilesetPath)) {
        if (m_pTiles->loadGraphics()) {
            resizeTileset(size());

            return true;
        }
    }

    //Tileset or graphics could not be loaded, try default
    if (m_pTiles->loadDefault()) {
        if (m_pTiles->loadGraphics()) {
            resizeTileset(size());
        }
    }

    return false;
}

bool GameWidget::setBackgroundPath(QString const &rBackgroundPath)
{
    if (m_pBackground->load(rBackgroundPath, width(), height())) {
        if (m_pBackground->loadGraphics()) {
            // Update the new background.
            updateBackground();

            return true;
        }
    }

    // Try default
    if (m_pBackground->loadDefault()) {
        if (m_pBackground->loadGraphics()) {
            // Update the new background.
            updateBackground();
        }
    }

    return false;
}

void GameWidget::resizeEvent(QResizeEvent *pEvent)
{
    if (pEvent->spontaneous() || m_pGameData == 0) {
        return;
    }

    resizeTileset(pEvent->size());
    m_pBackground->sizeChanged(m_pGameData->m_width / 2, m_pGameData->m_height / 2);
}

void GameWidget::resizeTileset(QSize const &rSize)
{
    if (m_pGameData == 0) {
        return;
    }

    QSize newtiles = m_pTiles->preferredTileSize(rSize, m_pGameData->m_width / 2,
        m_pGameData->m_height / 2);

    m_pTiles->reloadTileset(newtiles);
}

void GameWidget::setStatusText(QString const &rText)
{
    emit statusTextChanged(rText, m_lGameNumber);
}

void GameWidget::updateBackground()
{
    QPalette palette;
    palette.setBrush(backgroundRole(), m_pBackground->getBackground());
    setPalette(palette);
    setAutoFillBackground(true);
}
