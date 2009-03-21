/*
    Copyright (C) 2006 Mauricio Piacentini  <mauricio@tabuleiro.com>
    Modeled after qpoint.h
    Copyright (C) 1992-2006 Trolltech ASA. All rights reserved.

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

#ifndef TILECOORD_H
#define TILECOORD_H


#include <QHash>

/**
* This class implements
* 
* longer description
*
* @author Mauricio Piacentini  <mauricio@tabuleiro.com>
*/
class TileCoord
{
public:
    /**
    * Method description
    */
    TileCoord();
    /**
    * Method description
    * 
    * @param xpos blah blah
    * @param ypos blah blah
    * @param zpos blah blah
    */
    TileCoord(int xpos, int ypos, int zpos);
    /**
    * Method description
    *
    * @return @c true if reason?
    * @return @c false if reson
    */
    /**
    * Method description @return int */
    int x() const;
    /**
    * Method description @return int */
    int y() const;
    /**
    * Method description @return int */
    int z() const;
    /**
    * Method description @param x */
    void setX(int x);
    /**
    * Method description @param y */
    void setY(int y);
    /**
    * Method description @param y */
    void setZ(int y);

    /**
    * Method description
    */
    int &rx();
    /**
    * Method description @return int */
    int &ry();
    /**
    * Method description @return int */
    int &rz();
    /**
    * Method description
    *
    * @param t blah
    * @return TileCoord blah
    */
    TileCoord &operator+=(const TileCoord &t);
    /**
    * Method description
    *
    * @param t blah
    * @return TileCoord blah
    */ 
    TileCoord &operator-=(const TileCoord &t);
    /**
    * Method description
    *
    * @param c blah
    * @return TileCoord blah
    */
    TileCoord &operator*=(qreal c);
    /**
    * Method description
    *
    * @param c blah
    * @return TileCoord blah
    */
    TileCoord &operator/=(qreal c);
    /**
     * Method equals */
    friend inline bool operator==(const TileCoord &, const TileCoord &);
    /**
     * Method not equal */
    friend inline bool operator!=(const TileCoord &, const TileCoord &);
    /**
     * Method addition */
    friend inline const TileCoord operator+(const TileCoord &, const TileCoord &);
    /**
     * Method subtraction */
    friend inline const TileCoord operator-(const TileCoord &, const TileCoord &);
    /**
     * Method multiplication  */
    friend inline const TileCoord operator*(const TileCoord &, qreal);
    /**
     * Method multiplication */
    friend inline const TileCoord operator*(qreal, const TileCoord &);
    /**
     * Method subtraction */
    friend inline const TileCoord operator-(const TileCoord &);
    /**
     * Method division */
    friend inline const TileCoord operator/(const TileCoord &, qreal);

private:
    int yp;
    int xp;
    int zp;
};

inline uint qHash(const TileCoord &key) { return qHash(QString("X%1Y%2Z%3").arg(key.x()).arg(key.y()).arg(key.z())); }

inline TileCoord::TileCoord()
{ xp=0; yp=0; zp=0; }

inline TileCoord::TileCoord(int xpos, int ypos, int zpos)
{ xp = xpos; yp = ypos; zp = zpos; }

inline int TileCoord::x() const
{ return xp; }

inline int TileCoord::y() const
{ return yp; }

inline int TileCoord::z() const
{ return zp; }

inline void TileCoord::setX(int xpos)
{ xp = xpos; }

inline void TileCoord::setY(int ypos)
{ yp = ypos; }

inline void TileCoord::setZ(int zpos)
{ zp = zpos; }

inline int &TileCoord::rx()
{ return xp; }

inline int &TileCoord::ry()
{ return yp; }

inline int &TileCoord::rz()
{ return zp; }

inline TileCoord &TileCoord::operator+=(const TileCoord &p)
{ xp+=p.xp; yp+=p.yp; zp+=p.zp; return *this; }

inline TileCoord &TileCoord::operator-=(const TileCoord &p)
{ xp-=p.xp; yp-=p.yp; zp-=p.zp; return *this; }

inline TileCoord &TileCoord::operator*=(qreal c)
{ xp = qRound(xp*c); yp = qRound(yp*c); zp = qRound(zp*c); return *this; }

inline bool operator==(const TileCoord &p1, const TileCoord &p2)
{ return p1.xp == p2.xp && p1.yp == p2.yp && p1.zp == p2.zp; }

inline bool operator!=(const TileCoord &p1, const TileCoord &p2)
{ return p1.xp != p2.xp || p1.yp != p2.yp || p1.zp != p2.zp; }

inline const TileCoord operator+(const TileCoord &p1, const TileCoord &p2)
{ return TileCoord(p1.xp+p2.xp, p1.yp+p2.yp, p1.zp+p2.zp); }

inline const TileCoord operator-(const TileCoord &p1, const TileCoord &p2)
{ return TileCoord(p1.xp-p2.xp, p1.yp-p2.yp, p1.zp-p2.zp); }

inline const TileCoord operator*(const TileCoord &p, qreal c)
{ return TileCoord(qRound(p.xp*c), qRound(p.yp*c), qRound(p.zp*c)); }

inline const TileCoord operator*(qreal c, const TileCoord &p)
{ return TileCoord(qRound(p.xp*c), qRound(p.yp*c), qRound(p.zp*c)); }

inline const TileCoord operator-(const TileCoord &p)
{ return TileCoord(-p.xp, -p.yp, -p.zp); }

inline TileCoord &TileCoord::operator/=(qreal c)
{
    Q_ASSERT(!qFuzzyCompare(c, 0));
    xp = qRound(xp/c);
    yp = qRound(yp/c);
    zp = qRound(zp/c);
    return *this;
}

inline const TileCoord operator/(const TileCoord &p, qreal c)
{
    Q_ASSERT(!qFuzzyCompare(c, 0));
    return TileCoord(qRound(p.xp/c), qRound(p.yp/c), qRound(p.zp/c));
}

#endif // TILECOORD_H
