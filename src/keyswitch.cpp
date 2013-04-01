/***************************************************************************
                          keyswitch.cpp  -  description
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

#include "keyswitch.h"
#include "msgbox.h"


Keyswitch::Keyswitch(XKeyboard *keyboard, QWidget *parent) : QLabel(parent)
{
	keys=keyboard;
	QSettings * antico = new QSettings(QCoreApplication::applicationDirPath() + "/antico.cfg", QSettings::IniFormat, this);
	antico->beginGroup("Style");
	map_path = antico->value("path").toString()+"/language/";
	antico->endGroup(); //Style
	xkbConf = X11tools::loadXKBconf();
	if (xkbConf->status!=DONT_USE_XKB) {
		load_rules();
		qDebug()<<"XKB status : " <<xkbConf->status;
		if (xkbConf->status==USE_XKB)
			set_xkb();
		init();
		if (groupeName.count()>1 || xkbConf->showSingle) {
			draw_icon();
			createMenu();
		}
	}

}


void Keyswitch::init()
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

	connect(keys,SIGNAL(groupChanged(int)),this,SLOT(groupChange(int)));
	connect(keys,SIGNAL(layoutChanged()),this,SLOT(layoutChange()));
}

void Keyswitch::draw_icon()
{
	if (xkbConf->status==DONT_USE_XKB) {
		clear();
		return;
	}
	setToolTip(groupeName[currentGroup]);
	if (xkbConf->showFlag && QFile::exists(map_path + rule->layouts.key(groupeName[currentGroup])+".png")) {
		setPixmap(QPixmap(map_path + rule->layouts.key(groupeName[currentGroup])+".png"));

	} else {
		setText(groupeName[currentGroup]);

	}
}

void Keyswitch::update_style()
{

}

void Keyswitch::layoutChange()
{
	currentGroup=keys->getGroupNo();
	reconfigure();
}

void Keyswitch::groupChange(int index)
{
	if (xkbConf->status==DONT_USE_XKB)
		return;
	currentGroup=index;
	if (currentGroup<groupeName.size()-1)
		nextGroupe = currentGroup+1;
	else if (currentGroup == groupeName.size()-1)
		nextGroupe = 0;
	draw_icon();
	update();

}

void Keyswitch::mousePressEvent(QMouseEvent *event)
{
	if (xkbConf->status==DONT_USE_XKB ||xkbConf->status==ONLY_INDICATION)
		return;
	if (event->button() == Qt::LeftButton) {
		setNextGroupe();
	}
	if (event->button() == Qt::RightButton) {
		QPoint p = mapToGlobal(QPoint(0, 0));
		QSize s(contextMenu->sizeHint());
		p.setY(p.y()-s.height());
		contextMenu->exec(p);
	}

}

int Keyswitch::setKeyLayout(QString keyConf)
{
	QStringList argument = keyConf.split(" ");
	qDebug()<<"setxkbmap argumetns : "<< argument;
	int result = QProcess::execute("setxkbmap",argument);
	qDebug()<<" setxkbmap result : " << result;
	return result;
}

void Keyswitch::setNextGroupe()
{
	keys->setGroupNo(nextGroupe);
}

void Keyswitch::setPrevGroupe()
{
	nextGroupe=currentGroup;
	if (currentGroup-1<0)
		currentGroup = groupeName.size()-1;
	else
		currentGroup--;
	keys->setGroupNo(currentGroup);
}

void Keyswitch::createMenu()
{
	qDebug()<<"Create menu" << groupeName;
	contextMenu = new QMenu(this);
	qDebug()<<"Available groups" << groupeName;
	for (int index=0; index<groupeName.size(); index++) {
		QAction *act = new QAction(groupeName[index],this) ;
		act->setIcon(QIcon(map_path + rule->layouts.key(groupeName[index])+".png"));
		act->setData(groupeName[index]);
		contextMenu->addAction(act);
	}
	contextMenu->addSeparator();
	QAction *config = new QAction(tr("Configure"),this) ;
	config->setData("configure");
	contextMenu->addAction(config);
	connect(contextMenu,SIGNAL(triggered(QAction*)),SLOT(actionsActivate(QAction*)));
}

void  Keyswitch::reconfigure()
{
	QStringList tmpGrName;
	keys->getGroupNames(tmpGrName);

	XKBConf* newConf = X11tools::loadXKBconf();
	if (tmpGrName!=groupeName || newConf->layouts != xkbConf->layouts || newConf->showFlag != xkbConf->showFlag || newConf->showSingle != xkbConf->showSingle || newConf->status != xkbConf->status) {
		groupeName=tmpGrName;
		xkbConf=newConf;
		init();
		draw_icon();
		createMenu();
	}
}

void  Keyswitch::set_xkb()
{
	if (xkbConf->status==DONT_USE_XKB)
		return;
	model=xkbConf->model;
	for (int i=0; i<xkbConf->layouts.count(); i++) {
		layout+=xkbConf->layouts[i].layout;
		variant+=xkbConf->layouts[i].variant;
		if (i<xkbConf->layouts.count()-1) {
			layout+=",";
			variant+=",";
		}
	}
	qDebug()<<"Set layout : "<<layout;
	if (!(model.isEmpty() || model.isNull()) && !(layout.isEmpty() || layout.isNull())) {
		QString args = "-model "+model+" -layout "+layout;
		QString tmp = variant;
		if (!variant.isNull() && !variant.isEmpty() && !tmp.remove(",").isEmpty())
			args +=" -variant "+variant;
		if (!option.isNull() && !option.isEmpty())
			args +=" -option "+option;
		qDebug() << "XKB args " << args;
		if (setKeyLayout(args) == QProcess::CrashExit) {
			qDebug() << "XKB isn`t set";
			qDebug() << "------------------------------";
			return;
		} else
			qDebug() << "XKB  set";
	}
}

void  Keyswitch::actionsActivate(QAction * action)
{
	QString cmd = action->data().toString();
	if(cmd == "configure")
		configure();
	else
		keys->setGroupNo(groupeName.indexOf(cmd));
}

void  Keyswitch::configure()
{
	QDialog* config = new QDialog(this);
	QFrame  * frm = new QFrame(config);
	AnticoXKBconf* xkbconf = new AnticoXKBconf(frm,config);
	config->exec();
}

bool Keyswitch::load_rules()
{
	QString x11dir = X11tools::findX11Dir();
	if ( x11dir.isNull() || x11dir.isEmpty())
		return false;
	QString rulesFile = X11tools::findXkbRulesFile(x11dir,QX11Info::display());
	if ( rulesFile.isNull() || rulesFile.isEmpty())
		return false;

	rule = X11tools::loadRules(rulesFile,false);
	if (rule==NULL)
		return false;

	return true;

}
