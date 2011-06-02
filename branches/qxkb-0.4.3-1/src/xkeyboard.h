/***************************************************************************
                          xkeyboard.h  -  description
                             -------------------
    begin                : Sun Jul 8 2001
    copyright            : (C) 2001 by Leonid Zeitlin
    email                : lz@europe.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef XKEYBOARD_H
#define XKEYBOARD_H

#include "defs.h"

/**This class incapsulates XKeyboard Extension interface
  *@author Leonid Zeitlin
  */
  
class XKeyboard : public QObject {
  Q_OBJECT
private:
  int m_event_code;
  int m_numgroups;

  static XKeyboard *m_self;
  bool m_xkb_available;
public:
  static const int MaxNumKbdGroups = XkbNumKbdGroups;
  XKeyboard();
  ~XKeyboard();
  /** Set the current keyboard group to the given groupno */
  void setGroupNo(int groupno);
  /** Get the names of the currently configured keyboard groups */
  void getGroupNames(QStringList &list);
  static XKeyboard* self();
  /** return the current keyboard group index */
 int getGroupNo();
  /** return if XKEYBOARD extension is available in the X server */
  bool xkbAvailable() { return m_xkb_available; }
  int getNumKbdGroups() { return m_numgroups; };


  /** Examines an X Event passed to it and takes actions if the event is of
    * interest to XKeyboard */
  void processEvent(XEvent *ev);
private: // Private methods
  /** No descriptions */
  void retrieveNumKbdGroups();
signals: // Signals
  /** Signals that new keyboard group is selected */
  void groupChanged(int groupno);
  /** Signals that keyboard layout has changed and thus we need to reconfigure */
  void layoutChanged();
};

#endif
