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
#include "about.h"
#include "convert.h"



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
    public slots:
        void groupChange(int index);
        void layoutChange();
        void setNextGroupe();
        void setPrevGroupe();
        void trayClicked(QSystemTrayIcon::ActivationReason reason);

    protected slots:
        void reconfigure();
        void showClipboard();
        void presConvertKey();

    protected:
        int nextGroupe;
        void mousePressEvent(QMouseEvent *);
        int setKeyLayout(QString keyConf);
        void configure();
        void set_xkb();


    private:
        void draw_icon();
        void createMenu();
        bool load_rules();
        int getLayoutNumber();
        void convertString(QString content);
        void initKeyCodeMap();

        QxtGlobalShortcut *grabber;
        QHash<int, QString> stat_names; // qxkb stats names
        QHash<int, QString> event_names; // event names
        QHash< QString,int> app_lang;
        QHash<Window,int> window_lang;
        QHash<Window,QString> app_window;
        Window active_window;
        int currentGroup;
        QStringList groupeName;
        QStringList groupeFlag;
        QString flagsPath;
        XKeyboard *keys;
        QString map_path;
        QProcess *xkbmap;
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
        void shotcutConvert();
        void setLanguageMap(Window curent_window, int index);
        void checkLayoutChenge();
        void clearLangMap();
        AnticoXKBconf* xkbconf;
        QSvgRenderer *flagSVG;
        QString selectedString;


     private slots:
        void actionsActivate(QAction * action);
};

#endif // QXKB_H

