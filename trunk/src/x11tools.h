/***************************************************************************
                          x11tools.h  -  description
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


#ifndef X11TOOLS_H
#define X11TOOLS_H

#include "defs.h"

struct XkbOption;
struct XkbVariant;
struct LayoutUnit;

const int MAX_LABEL_LEN = 3;

enum {
    DONT_USE_XKB = 0,
    ONLY_INDICATION = 1,
    USE_XKB = 2,

};

enum {
    GLOBAL_LAYOUT = 0,
    DESK_LAYOUT=1,
    APP_LAYOUT = 2,
    WIN_LAYOUT = 3,

};

struct XkbOptionGroup {
	QString name;
	QString description;
	bool exclusive;
	QList<XkbOption> options;
};


struct XkbOption {
	QString name;
	QString description;
	// XkbOptionGroup* group;
};

struct RulesInfo {
	QHash<QString, QString> models;
	QHash<QString, QString> layouts;
	QHash<QString, XkbOption> options;
	QHash<QString, XkbOptionGroup> optionGroups;
};

struct XkbVariant {
	QString name;
	QString description;
};


inline QString createPair(QString key, QString value)
{
	if( value.isEmpty() )
		return key;
	return QString("%1(%2)").arg(key, value);
}

struct XKBConf {
	QString  model;
	QList<LayoutUnit> layouts;
	QStringList  options;
	int status;
	int switching;
	bool showFlag;
	bool showSingle;
	bool useConvert;
	bool lockKeys;
	QString shotcutConvert;

};

struct LayoutUnit {
private:
	QString displayName;
public:
	QString layout;
	QString variant;

	LayoutUnit() {}

	LayoutUnit(QString layout_, QString variant_):
		layout(layout_),
		variant(variant_)
	{}

	LayoutUnit(QString pair) {
		setFromPair( pair );
	}

	void setDisplayName(const QString& name) {
		displayName = name;
	}

	QString getDisplayName() const {
		return !displayName.isEmpty() ? displayName : getDefaultDisplayName(layout, variant);
	}

	void setFromPair(const QString& pair) {
		layout = parseLayout(pair);
		variant = parseVariant(pair);
	}

	QString toPair() const {
		return createPair(layout, variant);
	}
	/*
	        bool operator<(const LayoutUnit& lu) const {
	                return layout<lu.layout ||
	                                (layout==lu.layout && variant<lu.variant);
	        }
	*/
	bool operator!=(const LayoutUnit& lu) const {
		return layout!=lu.layout || variant!=lu.variant;
	}
	bool operator==(const LayoutUnit& lu) const {
		return layout==lu.layout && variant==lu.variant;
	}


	static QString getDefaultDisplayName(const QString& layout, const QString& variant="");

//private:
	static const QString parseLayout(const QString &layvar);
	static const QString parseVariant(const QString &layvar);
};


/*struct XkbConfig {
    QString model;
    QStringList options;
    QList<LayoutUnit> layouts;
};
*/
class X11tools
{
public:
	X11tools();
#ifndef HAVE_XKLAVIER
	/**
	 * Tries to find X11 xkb config dir
	 */
	static const QString findX11Dir();
	static const QString findXkbRulesFile(const QString &x11Dir, Display* dpy);
	static QList<XkbVariant> getVariants(const QString& layout, const QString& x11Dir);
	static RulesInfo* loadRules(const QString& rulesFile, bool layoutsOnly=false);
	static XKBConf getGroupNames(Display* dpy);
	static XKBConf* loadXKBconf();
	static void saveXKBconf(XKBConf* conf);
	static QString get_selected_text(XSelectionEvent *event);
	static KeySym unicodeToKeysys(QChar symbol);
	static Window getActiveWindowId();
	static QString getActiveWindowAppName(Window windowsId);
	static QHash<Window,QString> getWindowsList();
private:

	static XkbOptionGroup createMissingGroup(const QString& groupName);
	static bool isGroupExclusive(const QString& groupName);
#endif /* HAVE_XKLAVIER */
};

#endif // X11TOOLS_H


