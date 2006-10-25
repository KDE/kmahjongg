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

#include <QString>
#include <QHash>

class TileCoord
{
public:
    TileCoord();
    TileCoord(int xpos, int ypos, int zpos);

    bool isNull() const;

    int x() const;
    int y() const;
    int z() const;
    void setX(int x);
    void setY(int y);
    void setZ(int y);

    int &rx();
    int &ry();
    int &rz();

    TileCoord &operator+=(const TileCoord &t);
    TileCoord &operator-=(const TileCoord &t);
    TileCoord &operator*=(qreal c);
    TileCoord &operator/=(qreal c);

    friend inline bool operator==(const TileCoord &, const TileCoord &);
    friend inline bool operator!=(const TileCoord &, const TileCoord &);
    friend inline const TileCoord operator+(const TileCoord &, const TileCoord &);
    friend inline const TileCoord operator-(const TileCoord &, const TileCoord &);
    friend inline const TileCoord operator*(const TileCoord &, qreal);
    friend inline const TileCoord operator*(qreal, const TileCoord &);
    friend inline const TileCoord operator-(const TileCoord &);
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

inline bool TileCoord::isNull() const
{ return xp == 0 && yp == 0 && yp == 0; }

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
