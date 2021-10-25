/*
    SPDX-FileCopyrightText: 2007 Mauricio Piacentini <mauricio@tabuleiro.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KMAHJONGGLAYOUTSELECTOR_H
#define KMAHJONGGLAYOUTSELECTOR_H

// KF
#include <KConfigSkeleton>

// KMahjongg
#include "ui_gametype.h"

class GameView;
class GameData;
class GameScene;
class KMahjonggLayout;

class KMahjonggLayoutSelector : public QWidget, public Ui::KMahjonggLayoutSelector
{
    Q_OBJECT
public:
    explicit KMahjonggLayoutSelector(QWidget * parent, KConfigSkeleton * config);
    ~KMahjonggLayoutSelector() override;

public Q_SLOTS:
    void layoutChanged();
    void useRandomLayoutToggled(bool);

private:
    void setupData(KConfigSkeleton * config);

    QMap<QString, KMahjonggLayout *> layoutMap;
    GameScene * m_gameScene;
    GameData * m_gameData;
    GameView * m_gameView;
};

#endif
