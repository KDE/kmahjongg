/*
    SPDX-FileCopyrightText: 1997 Mathias Mueller <in5y158@public.uni-hamburg.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef BOARDLAYOUT_H
#define BOARDLAYOUT_H

// Qt
#include <QString>

// KMahjongg
#include "kmtypes.h"

const QString layoutMagic1_0 = QStringLiteral("kmahjongg-layout-v1.0");
const QString layoutMagic1_1 = QStringLiteral("kmahjongg-layout-v1.1");

/**
 * @short This class implements methods for loading and manipulating board
 *
 * @author Mauricio Piacentini  <mauricio@tabuleiro.com>
 */
class BoardLayout
{
public:
    BoardLayout();
    explicit BoardLayout(const BoardLayout & boardLayout);
    ~BoardLayout();

    void copyBoardLayout(UCHAR * to, unsigned short & numTiles) const;
    bool loadBoardLayout(const QString & from);
    bool saveBoardLayout(const QString & where) const;
    UCHAR getBoardData(short z, short y, short x) const;
    bool tileAbove(POSITION & p) const
    {
        return (tileAbove(p.z, p.y, p.x));
    }
    void deleteTile(POSITION & p);
    bool anyFilled(POSITION & p) const;
    bool allFilled(POSITION & p) const;
    void insertTile(POSITION & p);
    bool isTileAt(POSITION & p) const
    {
        return getBoardData(p.z, p.y, p.x) == '1';
    }

    void clearBoardLayout();
    void shiftLeft();
    void shiftRight();
    void shiftUp();
    void shiftDown();

    int getWidth() const
    {
        return m_width;
    }
    int getHeight() const
    {
        return m_height;
    }
    int getDepth() const
    {
        return m_depth;
    }

private:
    bool loadBoardLayout_10(const QString & from);
    void setBoardData(short z, short y, short x, UCHAR value);

    /**
     * is there a tile anywhere above here (top left to bot right quarter)
     */
    bool tileAbove(short z, short y, short x) const;

    void initialiseBoard();

    int m_width;
    int m_height;
    int m_depth;
    int m_maxTiles;

    QString m_filename;
    QByteArray m_loadedBoard;
    QByteArray m_board;
    unsigned short m_maxTileNum;

    static const QString layoutMagic1_0;
    static const QString layoutMagic1_1;
};

#endif
