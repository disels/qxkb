////////////////////////////////////////
//  File      : xkbconf.h             //
//  Written by: disels@gmail.com      //
//  Copyright : GPL                   //
////////////////////////////////////////

#ifndef XKBCONF_H
#define XKBCONF_H

#include "defs.h"
#include "x11tools.h"
#include "datamodels.h"
#include "ui_kswconfig.h"

class AnticoXKBconf : public QDialog
{
      Q_OBJECT
public:
    Ui_Form xkb_conf;
    AnticoXKBconf(QWidget* parent=0);
    ~AnticoXKBconf();
    void getHotKeys(XEvent *event);
    void clearHotKeys();

  protected:
     void closeEvent(QCloseEvent *event);

 signals:
    void saveConfig();

 public slots:
    void addLayout();
    void delLayout();
    void srcClick(QModelIndex index);
    void dstClick();
    void comboModelCh(int index);
    void comboVariantCh(int index);
    void statSelect(bool check);
    void apply();
    void setFlagUse();
    void setSinglShow();
    void layoutUp();
    void layoutDown();
    void updateOptionsCommand();
    void xkbOptionsChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight);
    void xkbShortcutPressed();
    void xkbShortcut3dPressed();
    void statSwitching(bool chek);

private:
    QString theme;
    QStringList listModels;
    RulesInfo * curRule ;
    SrcLayoutModel * srcLayoutModel;
    DstLayoutModel * dstLayoutModel;
    XkbOptionsModel* xkbOptionsModel;
    XKBConf* xkbConf;
    QList<XkbVariant> variants;
    QString ico_path;
    QString key ;
    QString  mods;
    QString hot_keys;
    bool load_rules();
    void setCmdLine();
    bool setStat();
    void initXKBTab();
    int getSelectedDstLayout();

};

#endif // XKBCONF_H
