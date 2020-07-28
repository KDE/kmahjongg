/* kmahjongg, the classic mahjongg game for KDE project
 *
 * Copyright (C) 1997 Mathias Mueller   <in5y158@public.uni-hamburg.de>
 * Copyright (C) 2006-2007 Mauricio Piacentini   <mauricio@tabuleiro.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA. */

// own
#include "gamedata.h"

// Qt
#include <QDataStream>

// KMahjongg
#include "boardlayout.h"
#include "kmahjongg_debug.h"
#include "prefs.h"

GameData::GameData(BoardLayout * boardlayout)
{
    m_width = boardlayout->getWidth();
    m_height = boardlayout->getHeight();
    m_depth = boardlayout->getDepth();
    m_maxTiles = (m_width * m_height * m_depth) / 4;

    m_highlight = QByteArray(m_width * m_height * m_depth, 0);
    m_mask = QByteArray(m_width * m_height * m_depth, 0);
    m_board = QByteArray(m_width * m_height * m_depth, 0);

    POSITION e; //constructor initializes it to 0

    m_moveList = QVector<POSITION>(m_maxTiles, e);
    m_tilePositions = QVector<POSITION>(m_maxTiles, e);
    m_posTable = QVector<POSITION>(m_maxTiles, e);
    m_positionDepends = QVector<DEPENDENCY>(m_maxTiles);

    //Copy board layout over
    boardlayout->copyBoardLayout((UCHAR *)getMaskBytes(), m_maxTileNum); //FIXME: is this cast safe?
}

GameData::~GameData()
{
}

void GameData::putTile(short z, short y, short x, UCHAR f)
{
    setBoardData(z, y, x, f);
    setBoardData(z, y + 1, x, f);
    setBoardData(z, y + 1, x + 1, f);
    setBoardData(z, y, x + 1, f);
}

bool GameData::tilePresent(int z, int y, int x) const
{
    if ((y < 0) || (x < 0) || (z < 0) || (y > m_height - 1) || (x > m_width - 1) || (z > m_depth - 1)) {
        return false;
    }

    return (BoardData(z, y, x) != 0 && MaskData(z, y, x) == '1');
}

UCHAR GameData::MaskData(short z, short y, short x) const
{
    if ((y < 0) || (x < 0) || (z < 0) || (y > m_height - 1) || (x > m_width - 1) || (z > m_depth - 1)) {
        return 0;
    }

    return m_mask.at((z * m_width * m_height) + (y * m_width) + x);
}

UCHAR GameData::HighlightData(short z, short y, short x) const
{
    if ((y < 0) || (x < 0) || (z < 0) || (y > m_height - 1) || (x > m_width - 1) || (z > m_depth - 1)) {
        return 0;
    }

    return m_highlight.at((z * m_width * m_height) + (y * m_width) + x);
}

UCHAR GameData::BoardData(short z, short y, short x) const
{
    if ((y < 0) || (x < 0) || (z < 0) || (y > m_height - 1) || (x > m_width - 1) || (z > m_depth - 1)) {
        return 0;
    }

    return m_board.at((z * m_width * m_height) + (y * m_width) + x);
}

void GameData::setBoardData(short z, short y, short x, UCHAR value)
{
    if ((y < 0) || (x < 0) || (z < 0) || (y > m_height - 1) || (x > m_width - 1)
        || (z > m_depth - 1)) {
        return;
    }

    m_board[(z * m_width * m_height) + (y * m_width) + x] = value;
}

POSITION & GameData::MoveListData(short i)
{
    if ((i >= m_moveList.size()) || (i < 0)) {
        qCDebug(KMAHJONGG_LOG) << "Attempt to access GameData::MoveListData with invalid index";
        i = 0;
    }

    return m_moveList[i];
}

void GameData::setMoveListData(short i, POSITION & value)
{
    if ((i >= m_moveList.size()) || (i < 0)) {
        return;
    }

    m_moveList[i] = value;
}

void GameData::generateTilePositions()
{
    // Generate the position data for the layout from contents of Game.Map.

    m_numTilesToGenerate = 0;

    for (int z = 0; z < m_depth; ++z) {
        for (int y = 0; y < m_height; ++y) {
            for (int x = 0; x < m_width; ++x) {
                setBoardData(z, y, x, 0);
                if (MaskData(z, y, x) == '1') {
                    m_tilePositions[m_numTilesToGenerate].x = x;
                    m_tilePositions[m_numTilesToGenerate].y = y;
                    m_tilePositions[m_numTilesToGenerate].z = z;
                    m_tilePositions[m_numTilesToGenerate].f = 254;
                    ++m_numTilesToGenerate;
                }
            }
        }
    }
}

void GameData::generatePositionDepends()
{
    // Generate the dependency data for the layout from the position data.
    // Note that the coordinates of each tile in tilePositions are those of
    // the upper left quarter of the tile.

    // For each tile,
    for (int i = 0; i < m_numTilesToGenerate; ++i) {
        // Get its basic position data
        int x = m_tilePositions[i].x;
        int y = m_tilePositions[i].y;
        int z = m_tilePositions[i].z;

        // LHS dependencies
        m_positionDepends[i].lhs_dep[0] = tileAt(x - 1, y, z);
        m_positionDepends[i].lhs_dep[1] = tileAt(x - 1, y + 1, z);

        // Make them unique
        if (m_positionDepends[i].lhs_dep[1] == m_positionDepends[i].lhs_dep[0]) {
            m_positionDepends[i].lhs_dep[1] = -1;
        }

        // RHS dependencies
        m_positionDepends[i].rhs_dep[0] = tileAt(x + 2, y, z);
        m_positionDepends[i].rhs_dep[1] = tileAt(x + 2, y + 1, z);

        // Make them unique
        if (m_positionDepends[i].rhs_dep[1] == m_positionDepends[i].rhs_dep[0]) {
            m_positionDepends[i].rhs_dep[1] = -1;
        }

        // Turn dependencies
        m_positionDepends[i].turn_dep[0] = tileAt(x, y, z + 1);
        m_positionDepends[i].turn_dep[1] = tileAt(x + 1, y, z + 1);
        m_positionDepends[i].turn_dep[2] = tileAt(x + 1, y + 1, z + 1);
        m_positionDepends[i].turn_dep[3] = tileAt(x, y + 1, z + 1);

        // Make them unique
        for (int j = 0; j < 3; ++j) {
            for (int k = j + 1; k < 4; ++k) {
                if (m_positionDepends[i].turn_dep[j] == m_positionDepends[i].turn_dep[k]) {
                    m_positionDepends[i].turn_dep[k] = -1;
                }
            }
        }

        // Placement dependencies
        m_positionDepends[i].place_dep[0] = tileAt(x, y, z - 1);
        m_positionDepends[i].place_dep[1] = tileAt(x + 1, y, z - 1);
        m_positionDepends[i].place_dep[2] = tileAt(x + 1, y + 1, z - 1);
        m_positionDepends[i].place_dep[3] = tileAt(x, y + 1, z - 1);

        // Make them unique
        for (int j = 0; j < 3; ++j) {
            for (int k = j + 1; k < 4; ++k) {
                if (m_positionDepends[i].place_dep[j] == m_positionDepends[i].place_dep[k]) {
                    m_positionDepends[i].place_dep[k] = -1;
                }
            }
        }

        // Filled and free indicators.
        m_positionDepends[i].filled = false;
        m_positionDepends[i].free = false;
    }
}

int GameData::tileAt(int x, int y, int z) const
{
    // x, y, z are the coordinates of a *quarter* tile.  This returns the
    // index (in positions) of the tile at those coordinates or -1 if there
    // is no tile at those coordinates.  Note that the coordinates of each
    // tile in positions are those of the upper left quarter of the tile.

    for (int i = 0; i < m_numTilesToGenerate; ++i) {
        if (m_tilePositions[i].z == z) {
            if ((m_tilePositions[i].x == x && m_tilePositions[i].y == y)
                || (m_tilePositions[i].x == x - 1 && m_tilePositions[i].y == y)
                || (m_tilePositions[i].x == x - 1 && m_tilePositions[i].y == y - 1)
                || (m_tilePositions[i].x == x && m_tilePositions[i].y == y - 1)) {
                return i;
            }
        }
    }
    return -1;
}

bool GameData::generateSolvableGame()
{
    // Initially we want to mark positions on layer 0 so that we have only
    // one free position per apparent horizontal line.
    for (int i = 0; i < m_numTilesToGenerate; ++i) {
        // Pick a random tile on layer 0
        int position, cnt = 0;

        do {
            position = random.bounded(m_numTilesToGenerate);

            if (cnt++ > (m_numTilesToGenerate * m_numTilesToGenerate)) {
                return false; // bail
            }
        } while (m_tilePositions[position].z != 0);

        // If there are no other free positions on the same apparent
        // horizontal line, we can mark that position as free.
        if (onlyFreeInLine(position)) {
            m_positionDepends[position].free = true;
        }
    }

    // Check to make sure we really got them all.  Very important for
    // this algorithm.
    for (int i = 0; i < m_numTilesToGenerate; ++i) {
        if (m_tilePositions[i].z == 0 && onlyFreeInLine(i)) {
            m_positionDepends[i].free = true;
        }
    }

    // Get ready to place the tiles
    int lastPosition = -1;
    int position = -1;
    int position2 = -1;

    // For each position,
    for (int i = 0; i < m_numTilesToGenerate; ++i) {
        // If this is the first tile in a 144 tile set,
        if ((i % 144) == 0) {
            // Initialise the faces to allocate. For the classic
            // dragon board there are 144 tiles. So we allocate and
            // randomise the assignment of 144 tiles. If there are > 144
            // tiles we will reallocate and re-randomise as we run out.
            // One advantage of this method is that the pairs to assign are
            // non-linear. In kmahjongg 0.4, If there were > 144 the same
            // allocation series was followed. So 154 = 144 + 10 rods.
            // 184 = 144 + 40 rods (20 pairs) which overwhemed the board
            // with rods and made deadlock games more likely.
            randomiseFaces();
        }

        // If this is the first half of a pair, there is no previous
        // position for the pair.
        if ((i & 1) == 0) {
            lastPosition = -1;
        }

        // Select a position for the tile, relative to the position of
        // the last tile placed.
        if ((position = selectPosition(lastPosition)) < 0) {
            return false; // bail
        }

        if (i < m_numTilesToGenerate - 1) {
            if ((position2 = selectPosition(lastPosition)) < 0) {
                return false; // bail
            }
            if (m_tilePositions[position2].z > m_tilePositions[position].z) {
                position = position2; // higher is better
            }
        }

        // Place the tile.
        placeTile(position, m_tilePair[i % 144]);

        // Remember the position
        lastPosition = position;
    }

    // The game is solvable.
    return true;
}

bool GameData::onlyFreeInLine(int position) const
{
    // Determines whether it is ok to mark this position as "free" because
    // there are no other positions marked "free" in its apparent horizontal
    // line.

    int i, i0, w;
    int lin, rin, out;

    QVector<int> nextLeft = QVector<int>(m_maxTiles, 0);
    QVector<int> nextRight = QVector<int>(m_maxTiles, 0);

    /* Check left, starting at position */
    lin = 0;
    out = 0;
    nextLeft[lin++] = position;

    do {
        w = nextLeft[out++];
        if (m_positionDepends[w].free || m_positionDepends[w].filled) {
            return false;
        }

        if ((i = m_positionDepends[w].lhs_dep[0]) != -1) {
            if (lin == m_maxTiles) {
                return false;
            }
            nextLeft[lin++] = i;
        }

        i0 = i;

        if ((i = m_positionDepends[w].lhs_dep[1]) != -1 && i0 != i) {
            if (lin == m_maxTiles) {
                return false;
            }
            nextLeft[lin++] = i;
        }
    } while (lin > out);

    /* Check right, starting at position */
    rin = 0;
    out = 0;
    nextRight[rin++] = position;

    do {
        w = nextRight[out++];

        if (m_positionDepends[w].free || m_positionDepends[w].filled) {
            return false;
        }

        if ((i = m_positionDepends[w].rhs_dep[0]) != -1) {
            if (rin == m_maxTiles) {
                return false;
            }
            nextRight[rin++] = i;
        }

        i0 = i;

        if ((i = m_positionDepends[w].rhs_dep[1]) != -1 && i0 != i) {
            if (rin == m_maxTiles) {
                return false;
            }
            nextRight[rin++] = i;
        }
    } while (rin > out);

    // Here, the position can be marked "free"
    return true;
}

int GameData::selectPosition(int lastPosition)
{
    int position, cnt = 0;
    bool goodPosition = false;

    // while a good position has not been found,
    while (!goodPosition) {
        // Select a random, but free, position.
        do {
            position = random.bounded(m_numTilesToGenerate);

            if (cnt++ > (m_numTilesToGenerate * m_numTilesToGenerate)) {
                return -1; // bail
            }
        } while (!m_positionDepends[position].free);

        // Found one.
        goodPosition = true;

        // If there is a previous position to take into account,
        if (lastPosition != -1) {
            // Check the new position against the last one.
            for (int i = 0; i < 4; ++i) {
                if (m_positionDepends[position].place_dep[i] == lastPosition) {
                    goodPosition = false; // not such a good position
                }
            }

            for (int i = 0; i < 2; ++i) {
                if ((m_positionDepends[position].lhs_dep[i] == lastPosition)
                    || (m_positionDepends[position].rhs_dep[i] == lastPosition)) {
                    goodPosition = false; // not such a good position
                }
            }
        }
    }
    return position;
}

void GameData::placeTile(int position, int tile)
{
    // Install the tile in the specified position
    m_tilePositions[position].f = tile;
    putTile(m_tilePositions[position]);

    // Update position dependency data
    m_positionDepends[position].filled = true;
    m_positionDepends[position].free = false;

    // Now examine the tiles near this to see if this makes them "free".
    int depend;

    for (int i = 0; i < 4; ++i) {
        if ((depend = m_positionDepends[position].turn_dep[i]) != -1) {
            updateDepend(depend);
        }
    }

    for (int i = 0; i < 2; ++i) {
        if ((depend = m_positionDepends[position].lhs_dep[i]) != -1) {
            updateDepend(depend);
        }

        if ((depend = m_positionDepends[position].rhs_dep[i]) != -1) {
            updateDepend(depend);
        }
    }
}

void GameData::updateDepend(int position)
{
    // Updates the free indicator in the dependency data for a position
    // based on whether the positions on which it depends are filled.

    // If the position is valid and not filled
    if (position >= 0 && !m_positionDepends[position].filled) {
        // Check placement depends.  If they are not filled, the
        // position cannot become free.
        int depend;
        for (int i = 0; i < 4; ++i) {
            if ((depend = m_positionDepends[position].place_dep[i]) != -1) {
                if (!m_positionDepends[depend].filled) {
                    return;
                }
            }
        }

        // If position is first free on apparent horizontal, it is
        // now free to be filled.
        if (onlyFreeInLine(position)) {
            m_positionDepends[position].free = true;
            return;
        }

        // Assume no LHS positions to fill
        bool lfilled = false;

        // If positions to LHS
        if ((m_positionDepends[position].lhs_dep[0] != -1)
            || (m_positionDepends[position].lhs_dep[1] != -1)) {
            // Assume LHS positions filled
            lfilled = true;

            for (int i = 0; i < 2; ++i) {
                if ((depend = m_positionDepends[position].lhs_dep[i]) != -1) {
                    if (!m_positionDepends[depend].filled) {
                        lfilled = false;
                    }
                }
            }
        }

        // Assume no RHS positions to fill
        bool rfilled = false;

        // If positions to RHS
        if ((m_positionDepends[position].rhs_dep[0] != -1)
            || (m_positionDepends[position].rhs_dep[1] != -1)) {
            // Assume LHS positions filled
            rfilled = true;

            for (int i = 0; i < 2; ++i) {
                if ((depend = m_positionDepends[position].rhs_dep[i]) != -1) {
                    if (!m_positionDepends[depend].filled) {
                        rfilled = false;
                    }
                }
            }
        }

        // If positions to left or right are filled, this position
        // is now free to be filled.
        m_positionDepends[position].free = (lfilled || rfilled);
    }
}

bool GameData::generateStartPosition2()
{
    // For each tile,
    for (int i = 0; i < m_numTilesToGenerate; ++i) {
        // Get its basic position data
        int x = m_tilePositions[i].x;
        int y = m_tilePositions[i].y;
        int z = m_tilePositions[i].z;

        // Clear Game.Board at that position
        setBoardData(z, y, x, 0);

        // Clear tile placed/free indicator(s).
        m_positionDepends[i].filled = false;
        m_positionDepends[i].free = false;

        // Set tile face blank
        m_tilePositions[i].f = 254;
    }

    // If solvable games should be generated,
    if (Prefs::solvableGames()) {
        if (generateSolvableGame()) {
            m_tileNum = m_maxTileNum;
            return true;
        } else {
            return false;
        }
    }

    // Initialise the faces to allocate. For the classic
    // dragon board there are 144 tiles. So we allocate and
    // randomise the assignment of 144 tiles. If there are > 144
    // tiles we will reallocate and re-randomise as we run out.
    // One advantage of this method is that the pairs to assign are
    // non-linear. In kmahjongg 0.4, If there were > 144 the same
    // allocation series was followed. So 154 = 144 + 10 rods.
    // 184 = 144 + 40 rods (20 pairs) which overwhemed the board
    // with rods and made deadlock games more likely.

    int remaining = m_numTilesToGenerate;
    randomiseFaces();

    for (int tile = 0; tile < m_numTilesToGenerate; tile += 2) {
        int p1;
        int p2;

        if (remaining > 2) {
            p2 = p1 = random.bounded(remaining - 2);
            int bail = 0;

            while (p1 == p2) {
                p2 = random.bounded(remaining - 2);

                if (bail >= 100) {
                    if (p1 != p2) {
                        break;
                    }
                }

                if ((m_tilePositions[p1].y == m_tilePositions[p2].y)
                    && (m_tilePositions[p1].z == m_tilePositions[p2].z)) {
                    // skip if on same y line
                    ++bail;
                    p2 = p1;

                    continue;
                }
            }
        } else {
            p1 = 0;
            p2 = 1;
        }

        POSITION a;
        POSITION b;

        a = m_tilePositions[p1];
        b = m_tilePositions[p2];

        m_tilePositions[p1] = m_tilePositions[remaining - 1];
        m_tilePositions[p2] = m_tilePositions[remaining - 2];

        remaining -= 2;

        getFaces(a, b);
        putTile(a);
        putTile(b);
    }

    m_tileNum = m_maxTileNum;

    return 1;
}

void GameData::getFaces(POSITION & a, POSITION & b)
{
    a.f = m_tilePair[m_tilesUsed];
    b.f = m_tilePair[m_tilesUsed + 1];
    m_tilesUsed += 2;

    if (m_tilesUsed >= 144) {
        randomiseFaces();
    }
}

void GameData::randomiseFaces()
{
    int nr;
    int numAlloced = 0;
    // stick in 144 tiles in pairsa.

    for (nr = 0; nr < 9 * 4; ++nr) {
        m_tilePair[numAlloced++] = TILE_CHARACTER + (nr / 4); // 4*9 Tiles
    }

    for (nr = 0; nr < 9 * 4; ++nr) {
        m_tilePair[numAlloced++] = TILE_BAMBOO + (nr / 4); // 4*9 Tiles
    }

    for (nr = 0; nr < 9 * 4; ++nr) {
        m_tilePair[numAlloced++] = TILE_ROD + (nr / 4); // 4*9 Tiles
    }

    for (nr = 0; nr < 4; ++nr) {
        m_tilePair[numAlloced++] = TILE_FLOWER + nr; // 4 Tiles
    }

    for (nr = 0; nr < 4; ++nr) {
        m_tilePair[numAlloced++] = TILE_SEASON + nr; // 4 Tiles
    }

    for (nr = 0; nr < 4 * 4; ++nr) {
        m_tilePair[numAlloced++] = TILE_WIND + (nr / 4); // 4*4 Tiles
    }

    for (nr = 0; nr < 3 * 4; ++nr) {
        m_tilePair[numAlloced++] = TILE_DRAGON + (nr / 4); // 3*4 Tiles
    }

    //randomise. Keep pairs together. Ie take two random
    //odd numbers (n,x) and swap n, n+1 with x, x+1

    int at = 0;
    for (int r = 0; r < 200; ++r) {
        int to = at;

        while (to == at) {
            to = random.bounded(144);

            if ((to & 1) != 0) {
                --to;
            }
        }

        UCHAR tmp = m_tilePair[at];
        m_tilePair[at] = m_tilePair[to];
        m_tilePair[to] = tmp;
        tmp = m_tilePair[at + 1];
        m_tilePair[at + 1] = m_tilePair[to + 1];
        m_tilePair[to + 1] = tmp;

        at += 2;

        if (at >= 144) {
            at = 0;
        }
    }

    m_tilesUsed = 0;
}

bool isFlower(UCHAR tile)
{
    return (tile >= TILE_FLOWER && tile <= TILE_FLOWER + 3);
}

bool isSeason(UCHAR tile)
{
    return (tile >= TILE_SEASON && tile <= TILE_SEASON + 3);
}

bool isBamboo(UCHAR tile)
{
    return (tile >= TILE_BAMBOO && tile < TILE_BAMBOO + 9);
}

bool isCharacter(UCHAR tile)
{
    return (tile < TILE_CHARACTER + 9);
}

bool isRod(UCHAR tile)
{
    return (tile >= TILE_ROD && tile < TILE_ROD + 9);
}

bool isDragon(UCHAR tile)
{
    return (tile >= TILE_DRAGON && tile < TILE_DRAGON + 3);
}

bool isWind(UCHAR tile)
{
    return (tile >= TILE_WIND && tile < TILE_WIND + 4);
}

bool GameData::isMatchingTile(POSITION & pos1, POSITION & pos2) const
{
    // don't compare 'equal' positions
    if (memcmp(&pos1, &pos2, sizeof(POSITION))) {
        UCHAR FA = BoardData(pos1.z, pos1.y, pos1.x);
        UCHAR FB = BoardData(pos2.z, pos2.y, pos2.x);

        if ((FA == FB) || (isFlower(FA) && isFlower(FB)) || (isSeason(FA) && isSeason(FB))) {
            return true;
        }
    }
    return false;
}

void GameData::setRemovedTilePair(POSITION & a, POSITION & b)
{
    if (isFlower(a.f)) {
        ++m_removedFlower[a.f - TILE_FLOWER];
        ++m_removedFlower[b.f - TILE_FLOWER];

        return;
    }

    if (isSeason(a.f)) {
        ++m_removedSeason[a.f - TILE_SEASON];
        ++m_removedSeason[b.f - TILE_SEASON];

        return;
    }

    if (isCharacter(a.f)) {
        m_removedCharacter[a.f - TILE_CHARACTER] += 2;

        return;
    }

    if (isBamboo(a.f)) {
        m_removedBamboo[a.f - TILE_BAMBOO] += 2;

        return;
    }

    if (isRod(a.f)) {
        m_removedRod[a.f - TILE_ROD] += 2;

        return;
    }

    if (isDragon(a.f)) {
        m_removedDragon[a.f - TILE_DRAGON] += 2;

        return;
    }

    if (isWind(a.f)) {
        m_removedWind[a.f - TILE_WIND] += 2;

        return;
    }
}

void GameData::clearRemovedTilePair(POSITION & a, POSITION & b)
{
    if (isFlower(a.f)) {
        --m_removedFlower[a.f - TILE_FLOWER];
        --m_removedFlower[b.f - TILE_FLOWER];

        return;
    }

    if (isSeason(a.f)) {
        --m_removedSeason[a.f - TILE_SEASON];
        --m_removedSeason[b.f - TILE_SEASON];

        return;
    }

    if (isCharacter(a.f)) {
        m_removedCharacter[a.f - TILE_CHARACTER] -= 2;

        return;
    }

    if (isBamboo(a.f)) {
        m_removedBamboo[a.f - TILE_BAMBOO] -= 2;

        return;
    }

    if (isRod(a.f)) {
        m_removedRod[a.f - TILE_ROD] -= 2;

        return;
    }

    if (isDragon(a.f)) {
        m_removedDragon[a.f - TILE_DRAGON] -= 2;

        return;
    }

    if (isWind(a.f)) {
        m_removedWind[a.f - TILE_WIND] -= 2;

        return;
    }
}

bool GameData::findMove(POSITION & posA, POSITION & posB)
{
    short posEnde = m_maxTileNum; // End of PosTable

    for (short z = 0; z < m_depth; ++z) {
        for (short y = 0; y < m_height - 1; ++y) {
            for (short x = 0; x < m_width - 1; ++x) {
                if (MaskData(z, y, x) != static_cast<UCHAR>('1')) {
                    continue;
                }

                if (!BoardData(z, y, x)) {
                    continue;
                }

                if (z < m_depth - 1) {
                    if (BoardData(z + 1, y, x) || BoardData(z + 1, y + 1, x)
                        || BoardData(z + 1, y, x + 1) || BoardData(z + 1, y + 1, x + 1)) {
                        continue;
                    }
                }

                if (x < m_width - 2 && (BoardData(z, y, x - 1) || BoardData(z, y + 1, x - 1))
                    && (BoardData(z, y, x + 2) || BoardData(z, y + 1, x + 2))) {
                    continue;
                }

                --posEnde;
                m_posTable[posEnde].z = z;
                m_posTable[posEnde].y = y;
                m_posTable[posEnde].x = x;
                m_posTable[posEnde].f = BoardData(z, y, x);
            }
        }
    }

    short posCount = 0; // store number of pairs found

    // The new tile layout with non-continuous horizantal spans
    // can lead to huge numbers of matching pairs being exposed.
    // we alter the loop to bail out when BoardLayout::maxTiles/2 pairs are found
    // (or less);
    while (posEnde < m_maxTileNum - 1 && posCount < m_maxTiles - 2) {
        for (short Pos = posEnde + 1; Pos < m_maxTileNum; ++Pos) {
            if (isMatchingTile(m_posTable[Pos], m_posTable[posEnde])) {
                if (posCount < m_maxTiles - 2) {
                    m_posTable[posCount++] = m_posTable[posEnde];
                    m_posTable[posCount++] = m_posTable[Pos];
                }
            }
        }
        ++posEnde;
    }

    if (posCount >= 2) {
        random.seed(QRandomGenerator::global()->generate()); // WABA: Why is the seed reset?
        const quint32 pos = random.bounded(posCount) & -2; // Even value
        posA = m_posTable[pos];
        posB = m_posTable[pos + 1];

        return true;
    } else {
        return false;
    }
}

int GameData::moveCount()
{
    short posEnde = m_maxTileNum; // End of PosTable

    for (short z = 0; z < m_depth; ++z) {
        for (short y = 0; y < m_height - 1; ++y) {
            for (short x = 0; x < m_width - 1; ++x) {
                if (MaskData(z, y, x) != static_cast<UCHAR>('1')) {
                    continue;
                }

                if (!BoardData(z, y, x)) {
                    continue;
                }

                if (z < m_depth - 1) {
                    if (BoardData(z + 1, y, x) || BoardData(z + 1, y + 1, x)
                        || BoardData(z + 1, y, x + 1) || BoardData(z + 1, y + 1, x + 1)) {
                        continue;
                    }
                }

                if (x < m_width - 2 && (BoardData(z, y, x - 1) || BoardData(z, y + 1, x - 1))
                    && (BoardData(z, y, x + 2) || BoardData(z, y + 1, x + 2))) {
                    continue;
                }

                --posEnde;
                m_posTable[posEnde].z = z;
                m_posTable[posEnde].y = y;
                m_posTable[posEnde].x = x;
                m_posTable[posEnde].f = BoardData(z, y, x);
            }
        }
    }

    short posCount = 0; // store number of pairs found

    while (posEnde < m_maxTileNum - 1 && posCount < m_maxTiles - 2) {
        for (short Pos = posEnde + 1; Pos < m_maxTileNum; ++Pos) {
            if (isMatchingTile(m_posTable[Pos], m_posTable[posEnde])) {
                if (posCount < m_maxTiles - 2) {
                    m_posTable[posCount++] = m_posTable[posEnde];
                    m_posTable[posCount++] = m_posTable[Pos];
                }
            }
        }

        ++posEnde;
    }

    return posCount / 2;
}

short GameData::findAllMatchingTiles(POSITION & posA)
{
    short pos = 0;

    for (short z = 0; z < m_depth; ++z) {
        for (short y = 0; y < m_height - 1; ++y) {
            for (short x = 0; x < m_width - 1; ++x) {
                if (MaskData(z, y, x) != static_cast<UCHAR>('1')) {
                    continue;
                }

                if (!BoardData(z, y, x)) {
                    continue;
                }

                if (z < m_depth - 1) {
                    if (BoardData(z + 1, y, x) || BoardData(z + 1, y + 1, x)
                        || BoardData(z + 1, y, x + 1) || BoardData(z + 1, y + 1, x + 1)) {
                        continue;
                    }
                }

                if (x < m_width - 2 && (BoardData(z, y, x - 1) || BoardData(z, y + 1, x - 1))
                    && (BoardData(z, y, x + 2) || BoardData(z, y + 1, x + 2))) {
                    continue;
                }

                m_posTable[pos].z = z;
                m_posTable[pos].y = y;
                m_posTable[pos].x = x;
                m_posTable[pos].f = BoardData(z, y, x);

                if (isMatchingTile(posA, m_posTable[pos])) {
                    ++pos;
                }
            }
        }
    }
    return pos;
}

bool GameData::loadFromStream(QDataStream & in)
{
    in >> m_board;
    in >> m_mask;
    in >> m_highlight;
    in >> m_allowUndo;
    in >> m_allowRedo;
    in >> m_tileNum;
    in >> m_maxTileNum;

    //Read list count
    in >> m_maxTiles;

    //Reconstruct the MoveList
    for (int i = 0; i < m_maxTiles; ++i) {
        POSITION thispos;
        in >> thispos.z;
        in >> thispos.y;
        in >> thispos.x;
        in >> thispos.f;
        setMoveListData(i, thispos);
    }
    return true;
}

bool GameData::saveToStream(QDataStream & out) const
{
    out << m_board;
    out << m_mask;
    out << m_highlight;
    out << m_allowUndo;
    out << m_allowRedo;
    out << m_tileNum;
    out << m_maxTileNum;

    //write the size of our lists
    out << m_maxTiles;

    //and then write all position components for the MoveList
    for (int i = 0; i < m_maxTiles; ++i) {
        POSITION thispos = m_moveList.at(i);
        out << static_cast<quint16>(thispos.z);
        out << static_cast<quint16>(thispos.y);
        out << static_cast<quint16>(thispos.x);
        out << static_cast<quint16>(thispos.f);
    }
    return true;
}

void GameData::shuffle()
{
    int count = 0;

    // copy positions and faces of the remaining tiles into
    // the pos table
    for (int z = 0; z < m_depth; ++z) {
        for (int y = 0; y < m_height; ++y) {
            for (int x = 0; x < m_width; ++x) {
                if (BoardData(z, y, x) && MaskData(z, y, x) == '1') {
                    m_posTable[count].z = z;
                    m_posTable[count].y = y;
                    m_posTable[count].x = x;
                    m_posTable[count].f = BoardData(z, y, x);
                    ++count;
                }
            }
        }
    }

    // now lets randomise the faces, selecting 400 pairs at random and
    // swapping the faces.
    for (int ran = 0; ran < 400; ++ran) {
        int pos1 = random.bounded(count);
        int pos2 = random.bounded(count);

        if (pos1 == pos2) {
            continue;
        }

        BYTE f = m_posTable[pos1].f;
        m_posTable[pos1].f = m_posTable[pos2].f;
        m_posTable[pos2].f = f;
    }

    // put the rearranged tiles back.
    for (int p = 0; p < count; ++p) {
        putTile(m_posTable[p]);
    }
}
