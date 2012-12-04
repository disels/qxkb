////////////////////////////////////////
//  File      : keyswitch.h           //
//  Written by: disels@gmail.com      //
//  Copyright : GPL                   //
////////////////////////////////////////

#ifndef KEYSWITCH_H
#define KEYSWITCH_H

#include "defs.h"
#include "xkeyboard.h"
#include "x11tools.h"
#include "xkbconf.h"


class Keyswitch : public QLabel
{
	Q_OBJECT
public:
	Keyswitch(XKeyboard *keyboard, QWidget *parent=0);
	void init();
	void update_style();

public slots:
	void groupChange(int index);
	void layoutChange();
	void setNextGroupe();
	void setPrevGroupe();

protected:
	int nextGroupe;
	void mousePressEvent(QMouseEvent *);
	int setKeyLayout(QString keyConf);
	void reconfigure();
	void configure();
	void set_xkb();


private:
	int currentGroup;
	QStringList groupeName;
	QStringList groupeFlag;
	QString flagsPath;
	void draw_icon();
	XKeyboard *keys;
	QString map_path;
	QProcess *xkbmap;
	QString model;
	QString layout;
	QString variant;
	QString option;
	QMenu *contextMenu;
	RulesInfo *rule;
	void createMenu();
	bool load_rules();
	XKBConf* xkbConf;

private slots:
	void actionsActivate(QAction * action);

};

#endif // KEYSWITCH_H
