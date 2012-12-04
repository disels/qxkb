////////////////////////////////////////
//  File      : qxkb.h                //
//  Written by: disels@gmail.com      //
//  Copyright : GPL                   //
////////////////////////////////////////


#ifndef QXKB_H
#define QXKB_H

#include "defs.h"
#include "xkeyboard.h"
#include "x11tools.h"
#include "xkbconf.h"
#include "convert.h"
#include "qxtglobalshortcut.h"


class QXKB : public QApplication
{
	Q_OBJECT


protected:
	virtual bool x11EventFilter(XEvent *);


public:
	QXKB(int &argc, char **argv);
	~QXKB();
	void init();
	bool firstStart();
	void setStartup();
	void set_event_names();
	void x11Hook();
public slots:
	void groupChange(int index);
	void layoutChange();
	void setNextGroupe();
	void setPrevGroupe();
	void trayClicked(QSystemTrayIcon::ActivationReason reason);

protected slots:
	void reconfigure();
	void showClipboard();

protected:
	void mousePressEvent(QMouseEvent *);
	int setKeyLayout(QString keyConf);
	void configure();
	void set_xkb();


private:
	void draw_icon();
	void createMenu();
	bool load_rules();
	int getLayoutNumber();
	void updateAppsLanglist();
	void cheklanguage();

	Window active_wm;
	QString active_app;
	QHash<int, QString> stat_names; // qxkb stats names
	QHash<int, QString> event_names; // event names
	QHash<QString,int > app_lang; // application language
	QHash<Window,int > win_lang; // window language
	QHash<Window,QString > win_to_app;
	//QHash<Window,int> window_lang;
	//QHash<Window,QString> app_window;
	Window active_window;
	int nextGroupe;
	int currentGroup;
	QStringList groupeName;
	QStringList groupeFlag;
	QString flagsPath;
	XKeyboard *keys;
	QString map_path;
	//QProcess *xkbmap;
	QString model;
	QString layout;
	QString variant;
	QString option;
	QMenu *contextMenu;
	RulesInfo *rule;
	XKBConf* xkbConf;
	QSystemTrayIcon*  trayIcon;
	QClipboard *clipboard;
	//symbol to keycode
	QMap<QString,int> lowwerSymbol;
	QMap<QString,int> upperSymbol;
	QXKBconf* xkbconf;
	//QSvgRenderer *flagSVG;
	QString selectedString;


private slots:
	void actionsActivate(QAction * action);
};

#endif // QXKB_H

