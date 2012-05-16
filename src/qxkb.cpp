////////////////////////////////////////
//  File      : qxkb.cpp              //
//  Written by: disels@gmail.com      //
//  Copyright : GPL                   //
////////////////////////////////////////

#include "qxkb.h"

QXKB::QXKB(int &argc, char **argv) : QApplication(argc, argv),
    contextMenu(0),
    rule(0),
    xkbConf(0),
    xkbconf(0)
{
    // mandatory to keep it running after closing about dialogs...
    setQuitOnLastWindowClosed(false);

    keys = new XKeyboard ();
    set_event_names();
    setWindowIcon(QIcon(":/about/qxkb.png"));
}


void QXKB::set_event_names()
{
    event_names.insert(0, "");
    event_names.insert(1, "");
    event_names.insert(2, "KeyPress");
    event_names.insert(3, "KeyRelease");
    event_names.insert(4, "ButtonPress");
    event_names.insert(5, "ButtonRelease");
    event_names.insert(6, "MotionNotify");
    event_names.insert(7, "EnterNotify");
    event_names.insert(8, "LeaveNotify");
    event_names.insert(9, "FocusIn");
    event_names.insert(10, "FocusOut");
    event_names.insert(11, "KeymapNotify");
    event_names.insert(12, "Expose");
    event_names.insert(13, "GraphicsExpose");
    event_names.insert(14, "NoExpose");
    event_names.insert(15, "VisibilityNotify");
    event_names.insert(16, "CreateNotify");
    event_names.insert(17, "DestroyNotify");
    event_names.insert(18, "UnmapNotify");
    event_names.insert(19, "MapNotify");
    event_names.insert(20, "MapRequest");
    event_names.insert(21, "ReparentNotify");
    event_names.insert(22, "ConfigureNotify");
    event_names.insert(23, "ConfigureRequest");
    event_names.insert(24, "GravityNotify");
    event_names.insert(25, "ResizeRequest");
    event_names.insert(26, "CirculateNotify");
    event_names.insert(27, "CirculateRequest");
    event_names.insert(28, "PropertyNotify");
    event_names.insert(29, "SelectionClear");
    event_names.insert(30, "SelectionRequest");
    event_names.insert(31, "SelectionNotify");
    event_names.insert(32, "ColormapNotify");
    event_names.insert(33, "ClientMessage");
    event_names.insert(34, "MappingNotify");

    stat_names.insert(0,"DONT_USE_XKB");
    stat_names.insert(1,"ONLY_INDICATION");
    stat_names.insert(2,"USE_XKB");
}

void QXKB::setStartup()
{
    // Display the config dialog only on first run.
    // It was really annoying to see it all time
    bool first = firstStart();

    QSettings qxkb(QDir::homePath() + "/.config/qxkb.cfg", QSettings::IniFormat, this);
    qxkb.beginGroup("Style");
    map_path = qxkb.value("path").toString()+"/language/";
    qxkb.endGroup(); //Style
    xkbConf = X11tools::loadXKBconf();
    trayIcon = new QSystemTrayIcon(this);
    load_rules();
    qDebug()<<"Use next status : "<< stat_names.value(xkbConf->status);
    if (xkbConf->status!=DONT_USE_XKB)
    {

        qDebug()<<" QXKB:XKB status : " <<xkbConf->status;
        if (xkbConf->status==USE_XKB)
            set_xkb();
        init();
        qDebug()<<" QXKB:Show single" << xkbConf->showSingle;
        if (groupeName.count()>1 || xkbConf->showSingle)
        {
            createMenu();
            draw_icon();

        }
        else
        {
            if (first)
                configure();

        }
    }
    else
    {
    //    if (first) show the cfg dialog whenthere is no tray icon
            configure();
    }
}

QXKB::~QXKB()
{
    if (keys)
        delete keys;
    if (xkbconf)
        delete xkbconf;
    if (contextMenu)
        delete contextMenu;
    if (xkbConf)
        delete xkbConf;
    //! \warning the trayIcon *has* to be deleted here to prevent XFreeColormap() free corruption.
    delete trayIcon;
}

bool QXKB::firstStart()
{
    if (!QFile::exists(QDir::homePath() + "/.config/qxkb.cfg"))
    {
        qDebug()  << " QXKB:Config file not find in :" <<QDir::homePath();
        qDebug()  << " QXKB:Create new";
        QSettings qxkb(QDir::homePath() + "/.config/qxkb.cfg", QSettings::IniFormat, this);
        QString themePath;
        QDir themeDir("/usr/share/qxkb/theme/default");
        if (themeDir.exists())
           themePath=themeDir.path();
        else
           themePath =   QCoreApplication::applicationDirPath()+"/theme/default";;
        qxkb.beginGroup("Style");
        map_path = themePath;
        qxkb.setValue("path",map_path);
        qxkb.endGroup(); //Style
        return true;
     }
    return false;
}

bool QXKB::x11EventFilter(XEvent *event)
{
    switch (((XKeyEvent *)event)->type)
    {

        default:
            keys->processEvent(event);
           return false;
    }
   return false;
}

void QXKB::init()
{
       groupeName.clear();
       if (xkbConf->status==DONT_USE_XKB)
           return;

       keys->getGroupNames(groupeName);
       currentGroup = keys->getGroupNo();
       if (currentGroup<groupeName.size()-1)
       nextGroupe = currentGroup+1;
       else if (currentGroup == groupeName.size()-1)
           nextGroupe = 0;
       clipboard=  QApplication::clipboard();
       connect(clipboard,SIGNAL(selectionChanged()),SLOT(showClipboard()));
       connect(keys,SIGNAL(groupChanged(int)),this,SLOT(groupChange(int)));
       connect(keys,SIGNAL(layoutChanged()),this,SLOT(layoutChange()));
 }

int QXKB::getLayoutNumber()
{
    XkbStateRec rec;
    XkbGetState(QX11Info::display(), XkbUseCoreKbd, &rec);
    return (int)rec.group;
}


void QXKB::showClipboard()
{
   selectedString= clipboard->text(QClipboard::Selection);
     qDebug()<<"Current selection: "<< selectedString;

}


void QXKB::draw_icon()
{
    if (groupeName.count()==0)
        return;
    if (xkbConf->status==DONT_USE_XKB)
    {
        qDebug() << "QXKB::draw_icon() exit";
        exit();
    }

    QString layout = xkbConf->layouts[currentGroup].layout;

   trayIcon->setContextMenu(contextMenu);
   connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));
   trayIcon->setToolTip(groupeName[currentGroup]);
   QString PNGfile = map_path +"/"+ layout+".png";
   QString SVGfile = map_path +"/"+ layout+".svg";

   bool havePNG =  QFile::exists(PNGfile );
   bool haveSVG =  QFile::exists( SVGfile);
    QSvgRenderer flagSVG(SVGfile);

   if (haveSVG)
   {
     haveSVG = haveSVG && flagSVG.isValid();
   }

    if (xkbConf->showFlag && (havePNG || haveSVG ))
     {
        if (haveSVG)
        {
            QPixmap pix(32,22);
            QPainter painter;
            painter.begin(&pix);
            flagSVG.render(&painter,QRectF(0,0,32,22));
            painter.end();
            trayIcon-> setIcon(QIcon(pix));
        }
        else if (havePNG)
          trayIcon-> setIcon(QIcon(PNGfile));


     }
     else
     {
        QFont font("Helvetica [Cronyx]",15);
        font.setBold(true);
        font.setLetterSpacing(QFont::PercentageSpacing,120);
        QPixmap pix(layout.length()*20,26);
        QPainter painter;
        painter.begin(&pix);
        painter.setFont(font);
        painter.fillRect(pix.rect(), Qt::darkGray);
        painter.setPen(Qt::white);
        painter.drawText(pix.rect(),Qt::AlignVCenter|Qt::AlignCenter,layout);
        painter.end();
        trayIcon-> setIcon(QIcon(pix));
     }
    if (xkbConf->status>1)
     {
        trayIcon->setContextMenu(contextMenu);
    }

    trayIcon->show();
}

void QXKB::trayClicked(QSystemTrayIcon::ActivationReason reason)
{
     switch (reason) {
     case QSystemTrayIcon::Trigger:
         setNextGroupe();
         break;
     case QSystemTrayIcon::DoubleClick:
         break;
     case QSystemTrayIcon::MiddleClick:
         setPrevGroupe();
         break;
     default:
         ;
     }
}

void QXKB::layoutChange()
{
    currentGroup=keys->getGroupNo();
    qDebug()<<"QXKB::layoutChange : reconfig";
    reconfigure();
    qDebug()<<"QXKB::layoutChange : done";
}

void QXKB::groupChange(int index)
{
    if (xkbConf->status==DONT_USE_XKB)
      return;
    currentGroup=index;
    if (currentGroup<groupeName.size()-1)
       nextGroupe = currentGroup+1;
    else if (currentGroup == groupeName.size()-1)
       nextGroupe = 0;
    draw_icon();

  }

int QXKB::setKeyLayout(QString keyConf)
{
    QStringList argument = keyConf.split(" ");
    qDebug()  << " QXKB:setxkbmap argumetns : "<< argument;
    int result = QProcess::execute("setxkbmap",argument);
    qDebug()  << " QXKB:setxkbmap result : " << result;
    return result;
}

void QXKB::setNextGroupe()
{
     keys->setGroupNo(nextGroupe);
}

void QXKB::setPrevGroupe()
{
     nextGroupe=currentGroup;
     if (currentGroup-1<0)
        currentGroup = groupeName.size()-1;
     else
        currentGroup--;
     keys->setGroupNo(currentGroup);
}

void QXKB::createMenu()
{
    qDebug()<<"Create menu" << groupeName;
    if (!contextMenu)
        contextMenu = new QMenu();
    else
        contextMenu->clear();

    qDebug()<<"Avalible groups" << groupeName;

    QHash<QString, QString> layouts = rule->layouts;

    for (int index=0;index<groupeName.size();index++)
       {
           QAction *act = new QAction(layouts.value(xkbConf->layouts[index].layout),this);
           QString PNGfile = map_path + xkbConf->layouts[index].layout+".png";
           QString SVGfile = map_path + xkbConf->layouts[index].layout+".svg";
            bool havePNG =  QFile::exists(PNGfile );
            bool haveSVG =  QFile::exists( SVGfile);

            if (haveSVG)
            {
                QSvgRenderer flagSVG(SVGfile);
                if ( haveSVG && flagSVG.isValid() )
                {
                    QPixmap pix(32,22);
                    QPainter painter;
                     painter.begin(&pix);
                     flagSVG.render(&painter,QRectF(0,0,32,22));
                     painter.end();
                    act->setIcon(QIcon(pix));
                }
                else
                    act->setIcon(QIcon(PNGfile));
            }
            else if (havePNG)
                act->setIcon(QIcon(PNGfile));

            act->setData(groupeName[index]);
        contextMenu->addAction(act);
        }

    contextMenu->addSeparator();

    QAction *config = new QAction(tr("Configure"),this) ;
    config->setData("configure");
    contextMenu->addAction(config);

    contextMenu->addSeparator();

    QAction *about_qt = new QAction(tr("About Qt"),this) ;
    about_qt->setData("about_qt");
    contextMenu->addAction(about_qt);

    QAction *about_qxkb = new QAction(tr("About QXKB"),this) ;
    about_qxkb->setData("about_qxkb");
    contextMenu->addAction(about_qxkb);

    contextMenu->addSeparator();

    QAction *qxkbExit = new QAction(tr("Exit"),this) ;
    qxkbExit->setData("exit");
    contextMenu->addAction(qxkbExit);

    connect(contextMenu,SIGNAL(triggered(QAction*)),SLOT(actionsActivate(QAction*)));
}

void  QXKB::reconfigure()
{
    QStringList tmpGrName;
    keys->getGroupNames(tmpGrName);

    XKBConf* newConf = X11tools::loadXKBconf();
    if (tmpGrName!=groupeName || newConf->layouts != xkbConf->layouts || newConf->showFlag != xkbConf->showFlag || newConf->showSingle != xkbConf->showSingle || newConf->status != xkbConf->status)
    {
        groupeName=tmpGrName;
        /*! \todo This prevent crashing when I remove one language from the
        destination list. QStringList then contains ("us", "cz", "") and the
        empty string causes a crash. There should be bettre way how to fix it imho... */
        groupeName.removeAll("");

        if (xkbConf)
        {
            delete xkbConf;
        }
        xkbConf=newConf;
        init();
        createMenu();
        draw_icon();
    }
    else
        delete newConf;
}

void  QXKB::set_xkb()
{
    if (xkbConf->status==DONT_USE_XKB)
           return;
    model=xkbConf->model;
    for (int i=0;i<xkbConf->layouts.count();i++)
    {
        layout+=xkbConf->layouts[i].layout;
        variant+=xkbConf->layouts[i].variant;
        if (i<xkbConf->layouts.count()-1)
        {
            layout+=",";
            variant+=",";
        }
    }
    option = xkbConf->options.join(",");
    qDebug()<<"Set layout : "<<layout;
    if (!(model.isEmpty() || model.isNull()) && !(layout.isEmpty() || layout.isNull()))
    {
        QString args = "-model "+model+" -layout "+layout;
        QString tmp = variant;
        if (!variant.isNull() && !variant.isEmpty() && !tmp.remove(",").isEmpty())
          args +=" -variant "+variant;
        if (!option.isNull() && !option.isEmpty())
        args +=" -option "+option;
        qDebug()  << " QXKB:XKB args " << args;
        if (setKeyLayout(args) == QProcess::CrashExit)
        {
            qDebug()  << " QXKB:XKB isn`t set";
            qDebug()  << " QXKB:------------------------------";
            return;
        }
        else
            qDebug()  << " QXKB:XKB  set";
    }
}

void QXKB::actionsActivate(QAction * action)
{
    QString cmd = action->data().toString();
    qDebug() << "QXKB::actionsActivate() command" << cmd;

    if(cmd == "configure")
    {
        configure();
    }
    else if (cmd == "about_qt")
    {
        aboutQt();
    }
    else if (cmd == "about_qxkb")
    {
        QMessageBox::about(0, tr("About QXKB"),
                            tr("<h2>QXKB, the keyboard layout switcher</h2>"
                               "<b>Version</b>: %1<p>"
                               "Gui tool to configure xkb extentions of x server.<p>"
                               "2009-2011(c) Fedor Chelbarakh, Petr Vanek ").arg(QXKB_VERSION));
    }
    else if (cmd == "exit")
    {
        qDebug() << "QXKB::actionsActivate() exit";
        exit(0);
    }
    else
    {
        keys->setGroupNo(groupeName.indexOf(cmd));
    }
}

void  QXKB::configure()
{
    if (!xkbconf)
    {
        xkbconf = new QXKBconf();
        xkbConf->lockKeys=true;
        connect(xkbconf,SIGNAL(saveConfig()),SLOT(reconfigure()));
    }
    xkbconf->exec();
}


bool QXKB::load_rules()
{
    /*Load aceptet keyboard language layouts and options from X11
      (like en,ru,ua etc.)
      and create list for configuration window
      Some code taken from kxkb project
      do no remove this
      */
    QString x11dir = X11tools::findX11Dir();
    if ( x11dir.isNull() || x11dir.isEmpty())
        return false;
     QString rulesFile = X11tools::findXkbRulesFile(x11dir,QX11Info::display());
     if ( rulesFile.isNull() || rulesFile.isEmpty())
        return false;

    if (rule)
       delete rule;
    rule = X11tools::loadRules(rulesFile,false);

    if (!rule)
        return false;

   return true;

}

