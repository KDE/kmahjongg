#include <kconfig.h>
#include "Preferences.h"


// Create the global instance
Preferences preferences;
static const char *statusBarKey = "StatusBar_visible";
static const char *shadowsKey = "Shadows_on";
static const char *miniTilesKey = "Use_mini_tiles";

static const char *saveKey = "Auto_save";
static const char *scaleKey = "Scale_mode";
static const char *tileKey = "Tileset_file";
static const char *backKey = "Background_file";
static const char *layoutKey = "Layout_file";
static const char *removedKey = "Show_removed";

static const char *tilefileDefault = "default.tileset";
static const char *backfileDefault = "default.bgnd";
static const char *layoutfileDefault = "default.layout";

Preferences::Preferences() {
  conf = 0;
}

Preferences::~Preferences() {

}

void Preferences::initialise(KConfig *c) {
    conf = c;
    conf->setGroup("");
    status = conf->readNumEntry(statusBarKey, TRUE );
    shadows = conf->readNumEntry( shadowsKey, FALSE);
    scale = conf->readNumEntry( scaleKey, FALSE);
    save = conf->readNumEntry( saveKey, TRUE);
    removed = conf->readNumEntry(removedKey, FALSE);	                              
    tileFile = conf->readEntry(tileKey, tilefileDefault);
    backFile = conf->readEntry(backKey, backfileDefault);
    layoutFile = conf->readEntry(layoutKey, layoutfileDefault);
    mini = conf->readNumEntry(miniTilesKey, FALSE);

}




void Preferences::sync(void) {
    conf->setGroup("");
    conf->writeEntry(statusBarKey, status );
    conf->writeEntry( shadowsKey, shadows);
    conf->writeEntry( miniTilesKey, mini);
    conf->writeEntry( scaleKey, scale);
    conf->writeEntry( saveKey, save);
    conf->writeEntry(tileKey, tileFile);
    conf->writeEntry(backKey, backFile);
    conf->writeEntry(layoutKey, layoutFile);
    conf->writeEntry(removedKey, removed);
}

QString Preferences::defaultBackground(void)
{
  return backfileDefault;
}

QString Preferences::defaultLayout(void)
{
  return layoutfileDefault;
}

QString Preferences::defaultTileset(void)
{
  return tilefileDefault;
}


