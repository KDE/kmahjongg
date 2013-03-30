/*
    Copyright (C) 1997 Mathias Mueller   <in5y158@public.uni-hamburg.de>

    Kmahjongg is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef BOARD_LAYOUT_H
#define BOARD_LAYOUT_H

#include <QString>
#include <QByteArray>
#include "KmTypes.h"

const QString layoutMagic1_0 = "kmahjongg-layout-v1.0";
const QString layoutMagic1_1 = "kmahjongg-layout-v1.1";

/**
 * @short This class implements methods for loading and manipulating board
 *
 * longer description
 *
 * @author Mauricio Piacentini  <mauricio@tabuleiro.com>
 */
class BoardLayout {

public:
    /**
     * Default Constructor
     */
    BoardLayout();

    /**
     * Copy constructor */
    BoardLayout(const BoardLayout &boardLayout);

    /**
     * Default Deconstructor
     */
    ~BoardLayout();
    /**
     * Method description
     *
     * @param from blah blah
     * @return @c true if ...
     * @return @c false if ...
     */
    bool loadBoardLayout(const QString &from);
    /**
     * Method description
     *
     * @param from blah blah
     * @return @c true if ...
     * @return @c false if ...
     * @see loadBoardLayout
     */
    bool loadBoardLayout_10(const QString &from);
    /**
     * Method description
     *
     * @param where blah blah
     * @return @c true if ...
     * @return @c false if ...
     * @see loadBoardLayout
     */
    bool saveBoardLayout(const QString &where);
    /**
     * Method description
     *
     * @param z blah blah
     * @param y blah blah
     * @param x blah blah
     * @return UCHAR ...
     */
    UCHAR getBoardData(short z, short y, short x);
    /**
     * Method description
     *
     * @param z blah blah
     * @param y blah blah
     * @param x blah blah
     * @param value blah blah
     */
    void setBoardData(short z, short y, short x, UCHAR value);

    /**
     * is there a tile anywhere above here (top left to bot right quarter)
     *
     * @param z blah blah
     * @param y blah blah
     * @param x blah blah
     * @return @c true if title abowe
     * @return @c false if title not abowe
     */
    bool tileAbove(short z, short y, short x);
    /**
     * is there a tile anywhere above here (top left to bot right quarter)
     *
     * @param &p blah blah @ref pos
     * @return @c true if title abowe
     * @return @c false if title not abowe
     * @see tileAbove
     */
    bool tileAbove(POSITION &p) { return(tileAbove(p.e, p.y, p.x));	}

    /**
     * is this tile blocked to the left or right
     *
     * @param z blah blah
     * @param y blah blah
     * @param x blah blah
     * @return @c true if ...
     * @return @c false if ...
     */
    bool blockedLeftOrRight(short z, short y, short x);
    /**
     * Description
     *
     * @param &p blah blah @ref pos
     */
    void deleteTile(POSITION &p);
    /**
     * Description
     *
     * @param &p blah blah @ref pos
     * @return @c true if title abowe
     * @return @c false if title not abowe
     */
    bool anyFilled(POSITION &p);
    /**
     * Description
     *
     * @param &p blah blah @ref pos
     * @return @c true if title abowe
     * @return @c false if title not abowe
     */
    bool allFilled(POSITION &p);
    /**
     * Description
     *
     * @param &p blah blah
     * @see pos
     */
    void insertTile(POSITION &p);
    /**
     * Description
     *
     * @param &p blah blah
     * @see pos
     * @return @c true if title abowe
     * @return @c false if title not abowe
     */
    bool isTileAt(POSITION &p) { return getBoardData(p.e, p.y, p.x) == '1'; }
    /**
     * Description
     *
     * @param *to blah blah
     * @param &numTiles blah blah
     */
    void copyBoardLayout(UCHAR *to , unsigned short &numTiles);
    /**
     * Method description
     */
    void clearBoardLayout();
    /**
     * Method description
     */
    void shiftLeft();
    /**
     * Method description
     */
    void shiftRight();
    /**
     * Method description
     */
    void shiftUp();
    /**
     * Method description
     */
    void shiftDown();

    /**
     * Get the loaded board. */
    QByteArray getLoadedBoard() const;

    /**
     * Get the board. */
    QByteArray getBoard() const;

    /**
     * Get max tile num. */
    unsigned short getMaxTileNum() const;

     int m_width;   /**< Member Description */
     int m_height;  /**< Member Description */
     int m_depth;   /**< Member Description */
     int m_maxTiles;/**< Member Description */
    /**
     * Get filename
     *
     * @return filename
     */
    QString getFilename() const;

protected:
    /**
     * Protected Method description
     */
    void initialiseBoard();

private:
    QString filename;       /**< Private Member Description */
    QByteArray loadedBoard; /**< Private Member Description */
    QByteArray board;       /**< Private Member Description */
    unsigned short maxTileNum; /**< Private Member Description */
};

#endif

