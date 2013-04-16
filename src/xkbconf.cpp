/***************************************************************************
                          xkbconf.cpp  -  description
                             -------------------
    begin                : Sun Sep 22 2010
    copyright            : (C) 2010 by Phoudor Chelbarakh, Petr Vanek
    email                : disels@gmail.com, yarpencz@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "xkbconf.h"

QXKBconf::QXKBconf(QWidget* parent) : QDialog(parent)
{
	xkb_conf.setupUi(this);
	// initialize the binding between list and stackedWidget
    xkb_conf.listWidget->setCurrentRow(0);
	QSettings qxkb(QDir::homePath() + "/.config/qxkb.cfg", QSettings::IniFormat, this);
	qxkb.beginGroup("Style");
	theme=qxkb.value("path").toString();
	ico_path = theme+"/language/";
	qxkb.endGroup(); //Style
	qDebug()<<"Theme " << theme;
	qDebug()<<"Icons " << ico_path;
	xkbConf = X11tools::loadXKBconf();
    if ( xkbConf->delay!= 0)
    {
        xkb_conf.sp_delay->setValue(xkbConf->delay);
    }
    else
    {
      xkbConf->delay = xkb_conf.sp_delay->value();
    }

    if ( xkbConf->repeat !=0)
    {
        xkb_conf.sp_rep->setValue(xkbConf->repeat);
    }
    else
    {
        xkbConf->repeat = xkb_conf.sp_rep->value();
    }

    connect(xkb_conf.sp_delay,SIGNAL(valueChanged(int)),this,SLOT(delay_change(int)));
    connect(xkb_conf.sp_rep,SIGNAL(valueChanged(int)),this,SLOT(rep_change(int)));
	connect(xkb_conf.buttonBox,SIGNAL(rejected()),this,SLOT(close()) );
	connect(xkb_conf.buttonBox,SIGNAL(accepted()) ,SLOT(apply()));
	connect(xkb_conf.radioButton,SIGNAL(clicked(bool)),SLOT(statSelect(bool)));
	connect(xkb_conf.radioButton_2,SIGNAL(clicked(bool)),SLOT(statSelect(bool)));
	connect(xkb_conf.radioButton_3,SIGNAL(clicked(bool)),SLOT(statSelect(bool)));

	connect(xkb_conf.rdBtnSwitchGlobal,SIGNAL(clicked(bool)),SLOT(switchSelect(bool)));
	connect(xkb_conf.rdBtnSwitchPerDesktop,SIGNAL(clicked(bool)),SLOT(switchSelect(bool)));
	connect(xkb_conf.rdBtnSwitchPerApp,SIGNAL(clicked(bool)),SLOT(switchSelect(bool)));
	connect(xkb_conf.rdBtnSwitchPerWin,SIGNAL(clicked(bool)),SLOT(switchSelect(bool)));


	xkb_conf.btnAdd->setEnabled(false);
	xkb_conf.btnRemove->setEnabled(false);
	xkb_conf.btnUp->setEnabled(false);
	xkb_conf.btnDown->setEnabled(false);
	xkb_conf.btnAdd->setIcon(QIcon::fromTheme(QString("arrow-right"),QIcon(theme+"/add.png")));
	xkb_conf.btnRemove->setIcon(QIcon::fromTheme(QString("arrow-left"),QIcon(theme+"/rem.png")));
	xkb_conf.btnUp->setIcon(QIcon::fromTheme(QString("arrow-up"), QIcon(theme+"/up.png")));
	xkb_conf.btnDown->setIcon(QIcon::fromTheme(QString("arrow-down"), QIcon(theme+"/down.png")));
	if (!setStat()) return;
	initXKBTab();
	xkbConf->lockKeys=true;
}

QXKBconf::~QXKBconf()
{
	delete xkbConf;
}

void QXKBconf::apply()
{
	X11tools::saveXKBconf(xkbConf);
    /*if (xkb_conf.stackedWidget->currentIndex()==0) {
		QStringList parm =  xkb_conf.editCmdLine->text().split(" ");
		parm.removeAt(0);
		qDebug()<<"Set XKB result"<< QProcess::execute("setxkbmap",parm);
        QStringList xparm;
        xparm.append("r");
        xparm.append("rate");
        xparm.append(QString(xkbConf->delay));
        xparm.append(QString(xkbConf->repeat));
        qDebug()<<"Set xSet result"<<QProcess::execute("xset",xparm);
    } else if (xkb_conf.stackedWidget->currentIndex()==2) {*/
		QStringList parm = xkb_conf.editCmdLineOpt->text().split(" ");
		parm.removeAt(0);
        qDebug()<<"Set XKB result"<<QProcess::execute("setxkbmap",parm);
        QStringList xparm;
        xparm.append("r");
        xparm.append("rate");
        xparm.append(QString::number(xkbConf->delay));
        xparm.append(QString::number(xkbConf->repeat));
        qDebug()<<"Set xSet result"<<QProcess::execute("xset",xparm);
    //}
	//qDebug()<<"Close";
	// close();
	emit saveConfig();

}

void QXKBconf::statSelect(bool /*check*/)
{
	if (xkb_conf.radioButton->isChecked())
		xkbConf->status=2;
	if (xkb_conf.radioButton_2->isChecked())
		xkbConf->status=1;
	if (xkb_conf.radioButton_3->isChecked())
		xkbConf->status=0;

	if (!setStat()) return;
	initXKBTab();
}



void QXKBconf::switchSelect(bool /*check*/)
{
	if (xkb_conf.rdBtnSwitchGlobal->isChecked())
		xkbConf->switching=GLOBAL_LAYOUT;
	if (xkb_conf.rdBtnSwitchPerDesktop->isChecked())
		xkbConf->switching=DESK_LAYOUT;
	if (xkb_conf.rdBtnSwitchPerApp->isChecked())
		xkbConf->switching=APP_LAYOUT;
	if (xkb_conf.rdBtnSwitchPerWin->isChecked())
		xkbConf->switching=WIN_LAYOUT;
}



void QXKBconf::initXKBTab()
{

	connect(xkb_conf.btnAdd,SIGNAL(clicked()),SLOT(addLayout()));
	connect(xkb_conf.btnRemove,SIGNAL(clicked()),SLOT(delLayout()));
	load_rules();
	//set comboModel list
	QHashIterator<QString, QString> i(curRule->models);
	while (i.hasNext()) {
		i.next();
		listModels<<i.value();
	}
	// set avaleble language
	srcLayoutModel = new SrcLayoutModel(curRule,ico_path,this);
	qDebug()<<(ico_path);
	dstLayoutModel = new DstLayoutModel(curRule,xkbConf,ico_path,this);
	xkbOptionsModel = new XkbOptionsModel(curRule,xkbConf,this);
	xkb_conf.comboModel->addItems(listModels);
	xkb_conf.comboModel->setCurrentIndex(listModels.indexOf(curRule->models.value(xkbConf->model)));
	xkb_conf.srcTableView->setModel(srcLayoutModel);
	xkb_conf.srcTableView->resizeColumnToContents(0);
	xkb_conf.dstTableView->setModel(dstLayoutModel);
	xkb_conf.dstTableView->resizeColumnToContents(0);
	xkb_conf.xkbOptionsTreeView->setModel(xkbOptionsModel);
	xkb_conf.xkbOptionsTreeView->header()->hide();
	xkb_conf.xkbOptionsTreeView->expandAll();
	for (int ii=0; ii<xkbConf->options.count(); ii++) {
		if (xkbConf->options[ii].contains("grp:"))
			xkb_conf.btnXkbShortcut->setText(tr("Defined"));
		if (xkbConf->options[ii].contains("lv3:"))
			xkb_conf.btnXkbShortcut3d->setText(tr("Defined"));
	}

	switch (xkbConf->switching) {
	case GLOBAL_LAYOUT:
		xkb_conf.rdBtnSwitchGlobal->setChecked(true);
		break;
	case APP_LAYOUT:
		xkb_conf.rdBtnSwitchPerApp->setChecked(true);
		break;
	case WIN_LAYOUT:
		xkb_conf.rdBtnSwitchPerWin->setChecked(true);
		break;
	}

	connect(xkb_conf.srcTableView,SIGNAL(clicked (const QModelIndex &)),SLOT(srcClick(QModelIndex)));
	connect(xkb_conf.dstTableView->selectionModel(),SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),SLOT(dstClick()));
	connect(xkb_conf.comboModel,SIGNAL(currentIndexChanged (int)) ,SLOT(comboModelCh(int)));
	connect(xkb_conf.comboVariant,SIGNAL(currentIndexChanged (int)),SLOT(comboVariantCh(int)));
	connect(xkb_conf.btnUp,SIGNAL(clicked()),SLOT(layoutUp()));
	connect(xkb_conf.btnDown,SIGNAL(clicked()),SLOT(layoutDown()));
	connect(xkbOptionsModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),SLOT(xkbOptionsChanged(const QModelIndex &, const QModelIndex &)));
	connect( xkb_conf.btnXkbShortcut, SIGNAL(clicked()),SLOT(xkbShortcutPressed()));
	connect(xkb_conf.btnXkbShortcut3d, SIGNAL(clicked()), SLOT(xkbShortcut3dPressed()));


	setCmdLine();
	updateOptionsCommand();


}

bool QXKBconf::setStat()
{
	switch (xkbConf->status ) {
	case DONT_USE_XKB:

		xkb_conf.grpIndicatorOptions->setEnabled(false);
		xkb_conf.grpLayouts->setEnabled(false);
		xkb_conf.page_2->setEnabled(false);
		xkb_conf.page_3->setEnabled(false);
		xkb_conf.radioButton->setChecked(false);
		xkb_conf.radioButton_2->setChecked(false);
		xkb_conf.radioButton_3->setChecked(true);
		return false;

	case ONLY_INDICATION:

		xkb_conf.grpIndicatorOptions->setEnabled(true);
		xkb_conf.grpLayouts->setEnabled(false);
		xkb_conf.page_2->setEnabled(false);
		xkb_conf.page_3->setEnabled(false);
		xkb_conf.radioButton->setChecked(false);
		xkb_conf.radioButton_2->setChecked(true);
		xkb_conf.radioButton_3->setChecked(false);
		xkb_conf.chkShowFlag->setChecked(xkbConf->showFlag);
		xkb_conf.chkShowSingle->setChecked(xkbConf->showSingle);
		connect(xkb_conf.chkShowFlag,SIGNAL(clicked()),SLOT(setFlagUse()));
		connect(xkb_conf.chkShowSingle,SIGNAL(clicked()),SLOT(setSinglShow()));
		return false;

	case USE_XKB:

		xkb_conf.grpIndicatorOptions->setEnabled(true);
		xkb_conf.grpLayouts->setEnabled(true);
		xkb_conf.page_2->setEnabled(true);//not work yet
		xkb_conf.page_3->setEnabled(true);
		xkb_conf.radioButton->setChecked(true);
		xkb_conf.radioButton_2->setChecked(false);
		xkb_conf.radioButton_3->setChecked(false);
		xkb_conf.chkShowFlag->setChecked(xkbConf->showFlag);
		xkb_conf.chkShowSingle->setChecked(xkbConf->showSingle);
		connect(xkb_conf.chkShowFlag,SIGNAL(clicked()),SLOT(setFlagUse()));
		connect(xkb_conf.chkShowSingle,SIGNAL(clicked()),SLOT(setSinglShow()));
		return true;
	}
	return false;
}

void QXKBconf::setCmdLine()
{
	QString cmdLine="setxkbmap -model " + xkbConf->model;
	cmdLine +=" -layout ";
	for(int i=0; i<xkbConf->layouts.size(); i++) {
		cmdLine+=xkbConf->layouts[i].layout;
		if (i<xkbConf->layouts.size()-1)
			cmdLine+=",";
	}
	cmdLine+=" -variant ";
	for(int i=0; i<xkbConf->layouts.size(); i++) {
		cmdLine+=xkbConf->layouts[i].variant;
		if (i<xkbConf->layouts.size()-1)
			cmdLine+=",";
	}
	xkb_conf.editCmdLine->setText(cmdLine);
}

void QXKBconf::addLayout()
{
	QItemSelectionModel* selectionModel = xkb_conf.srcTableView->selectionModel();
	if( selectionModel == NULL || !selectionModel->hasSelection()
	    || xkbConf->layouts.size() >= 4 )
		return;

	QModelIndexList selected = selectionModel->selectedRows();
	qDebug()<<"selected : " <<selected;
	QString layout = srcLayoutModel->getLayoutAt(selected[0].row());
	if (layout.isEmpty() || xkbConf->layouts.contains(layout))
		return;
	LayoutUnit lu;
	lu.layout=layout;
	xkbConf->layouts.append(lu);
	dstLayoutModel->reset();
	xkb_conf.dstTableView->update();
	setCmdLine();

}

void QXKBconf::delLayout()
{
	QItemSelectionModel* selectionModel = xkb_conf.dstTableView->selectionModel();
	if( selectionModel == NULL || !selectionModel->hasSelection()|| xkbConf->layouts.size() == 0 )
		return;
	QModelIndexList selected = selectionModel->selectedRows();
	xkbConf->layouts.removeAt(selected[0].row());
	dstLayoutModel->reset();
	xkb_conf.dstTableView->update();
	setCmdLine();
}

void QXKBconf::srcClick(QModelIndex /*index*/)
{
	xkb_conf.btnAdd->setEnabled(true);
}

void QXKBconf::dstClick()
{

	int row = getSelectedDstLayout();
	xkb_conf.btnRemove->setEnabled(row != -1);
	int el_count =xkb_conf.comboVariant->count();
	for (int i=el_count; i>0; i--) {
		xkb_conf.comboVariant->removeItem(i);
	}
	// xkb_conf.comboVariant->clear(); some sheet seg fail in use

	xkb_conf.comboVariant->setEnabled( row != -1 );
	xkb_conf.btnUp->setEnabled(row != -1);
	xkb_conf.btnDown->setEnabled(row != -1);
	if( row == -1 ) {
		return;
	}

	QString layout=xkbConf->layouts[row].layout;
	qDebug()<<layout;

	variants = X11tools::getVariants(layout,X11tools::findX11Dir());
	qDebug()<<"QXKBconf:addItem ";

	if (xkb_conf.comboVariant->count()==0)
		xkb_conf.comboVariant->addItem(tr("Default"),"Default");
	xkb_conf.comboVariant->setItemData(0, tr("Default"), Qt::ToolTipRole );
	xkb_conf.comboVariant->setItemText(0, tr("Default"));

	if (variants.count()>0) {
		for (int i=0; i<variants.count(); i++) {
			xkb_conf.comboVariant->addItem(variants[i].description,variants[i].name);
			xkb_conf.comboVariant->setItemData(xkb_conf.comboVariant->count()-1, variants[i].description, Qt::ToolTipRole );
		}
		QString variant = xkbConf->layouts[row].variant;
		if( variant != NULL && !variant.isEmpty() ) {
			int idx = xkb_conf.comboVariant->findData(variant);
			xkb_conf.comboVariant->setCurrentIndex(idx);
		} else {
			xkb_conf.comboVariant->setCurrentIndex(0);
		}
	}
}

bool QXKBconf::load_rules()
{
	QString x11dir = X11tools::findX11Dir();
	if ( x11dir.isNull() || x11dir.isEmpty())
		return false;
	QString rulesFile = X11tools::findXkbRulesFile(x11dir,QX11Info::display());
	if ( rulesFile.isNull() || rulesFile.isEmpty())
		return false;

	curRule = X11tools::loadRules(rulesFile,false);
	if (curRule==NULL)
		return false;
	return true;

}

void QXKBconf::comboModelCh(int index)
{
	xkbConf->model= curRule->models.key( xkb_conf.comboModel->itemText(index));
	setCmdLine();
}

void QXKBconf::comboVariantCh(int index)
{
	if (0==index)
		xkbConf->layouts[getSelectedDstLayout()].variant="";
	else if (index < variants.count()) // it sometimes crashed here with "index out of bounds"
		xkbConf->layouts[getSelectedDstLayout()].variant=variants.at(index-1).name;
	setCmdLine();
}

int QXKBconf::getSelectedDstLayout()
{
	QItemSelectionModel* selectionModel = xkb_conf.dstTableView->selectionModel();
	if( selectionModel == NULL || !selectionModel->hasSelection() )
		return -1;

	QModelIndexList selected = selectionModel->selectedRows();
	int row = selected.count() > 0 ? selected[0].row() : -1;
	return row;
}

void QXKBconf::setFlagUse()
{
	xkbConf->showFlag=xkb_conf.chkShowFlag->isChecked();
}

void QXKBconf::setSinglShow()
{
	xkbConf->showSingle=xkb_conf.chkShowSingle->isChecked();
}

void QXKBconf::layoutUp()
{
	int row = getSelectedDstLayout();
	if (row>0) {
		LayoutUnit lu = xkbConf->layouts[row-1];
		xkbConf->layouts[row-1]=xkbConf->layouts[row];
		xkbConf->layouts[row]=lu;
		dstLayoutModel->reset();
		xkb_conf.dstTableView->update();
		setCmdLine();
	}
}

void QXKBconf::layoutDown()
{
	int row = getSelectedDstLayout();
	if (row<xkbConf->layouts.count()-1 && row>-1) {
		LayoutUnit lu = xkbConf->layouts[row+1];
		xkbConf->layouts[row+1]=xkbConf->layouts[row];
		xkbConf->layouts[row]=lu;
		dstLayoutModel->reset();
		setCmdLine();
	}

}

void QXKBconf::updateOptionsCommand()
{

	QString cmd = "setxkbmap";
	if( xkb_conf.checkResetOld->isChecked())
		cmd += " -option";

	if( ! xkbConf->options.empty() ) {
		cmd += " -option ";
		cmd +=xkbConf->options.join(",");
	}

	xkb_conf.editCmdLineOpt->setText(cmd);
}

void QXKBconf::xkbOptionsChanged(const QModelIndex & /*topLeft*/, const QModelIndex & /*bottomRight*/)
{
	updateOptionsCommand();

}

void QXKBconf::closeEvent(QCloseEvent *event)
{
	xkbConf->lockKeys=false;
	hide();
	event->ignore();
}

void QXKBconf::xkbShortcutPressed()
{
	xkb_conf.stackedWidget->setCurrentIndex(2);
    xkb_conf.listWidget->setCurrentRow(2);
	xkbOptionsModel->gotoGroup("grp", xkb_conf.xkbOptionsTreeView);
}

void QXKBconf::xkbShortcut3dPressed()
{
    xkb_conf.listWidget->setCurrentRow(2);
	xkb_conf.stackedWidget->setCurrentIndex(2);
	xkbOptionsModel->gotoGroup("lv3", xkb_conf.xkbOptionsTreeView);
}

void QXKBconf::statSwitching(bool /*check*/)
{
	if (xkb_conf.rdBtnSwitchGlobal->isChecked())
		xkbConf->switching=2;
	if (xkb_conf.rdBtnSwitchPerApp->isChecked())
		xkbConf->switching=1;
	if (xkb_conf.rdBtnSwitchPerWin->isChecked())
		xkbConf->switching=0;
}


void QXKBconf::getHotKeys(XEvent *event)
{
	XKeyEvent  *keys = (XKeyEvent *)event;
	switch (XkbKeycodeToKeysym(QX11Info::display(), keys->keycode,0,0)) {
	case XK_Shift_L :
	case XK_Shift_R :
		mods = "Shift";
		key.clear();
		break;
	case XK_Control_L :
	case XK_Control_R :
		mods = "Ctrl";
		key.clear();
		break;
	case XK_Alt_L :
	case XK_Alt_R :
		mods = "Alt";
		key.clear();
		break;
	default:
		key = QString(XKeysymToString(XkbKeycodeToKeysym(QX11Info::display(), keys->keycode,0,0)));
		if (key.count()<2) key = key.toUpper();

	}
	if (!mods.isEmpty()) {
		hot_keys =mods +"+"+key;
	} else if (key.count()>1) {
		hot_keys =key;
	}

}

void QXKBconf::clearHotKeys()
{
	qDebug()<<"QXKBconf:: Kurrent mods "<<mods;
	if (mods.isEmpty()) return;
	qDebug()<<"QXKBconf:: Kurrent key "<<key;
	if (!key.isEmpty()) return;
	mods.clear();
}

void QXKBconf::delay_change(int new_dalay)
{
    xkbConf->delay=new_dalay;
}

void QXKBconf::rep_change(int new_rep)
{
    xkbConf->repeat = new_rep;
}
