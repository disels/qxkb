/***************************************************************************
                          xkbconf.h  -  description
                             -------------------
    begin                : Sun Sep 22 2010
    copyright            : (C) 2010 by Phoudor Chelbarakh
    email                : disels@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef XKBCONF_H
#define XKBCONF_H

#include "defs.h"
#include "x11tools.h"
#include "datamodels.h"
#include "ui_kswconfig.h"
#undef Bool

class QXKBconf : public QDialog
{
	Q_OBJECT
public:
	Ui_Form xkb_conf;
	QXKBconf(QWidget* parent=0);
	~QXKBconf();
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
	void switchSelect(bool chek);
    void delay_change(int new_dalay);
    void rep_change(int rep_dalay);

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
