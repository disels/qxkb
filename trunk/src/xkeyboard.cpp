/***************************************************************************
                          xkeyboard.cpp  -  description
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

#include "xkeyboard.h"


XKeyboard *XKeyboard::m_self = 0;

XKeyboard::XKeyboard()
{
  Display *display = QX11Info::display();
  XSelectInput(display, DefaultRootWindow(display), SubstructureNotifyMask);
  int opcode, errorBase, major = XkbMajorVersion, minor = XkbMinorVersion;

  // check the library version
  if (!XkbLibraryVersion(&major, &minor)) {
    qWarning()<<tr("This program was built against XKB extension library\n"
      "version %1.%2, but is run with the library version %3.%4.\n"
      "This may cause various problems and even result in a complete\n"
      "failure to function\n").arg(XkbMajorVersion).arg(XkbMinorVersion).arg(major).arg(minor);
  }

  // initialize the extension
  m_xkb_available = XkbQueryExtension(display, &opcode, &m_event_code, &errorBase, &major, &minor);
  if (!m_xkb_available) {
     qCritical() << tr("The X Server does not support a compatible XKB extension.\n"
      "Either the server is not XKB-capable or the extension was disabled.\n"
      "This program would not work with this server, so it will exit now\n");
  }
  else {
    // register for XKB events
    //// group state change, i.e. the current group changed:
    XkbSelectEventDetails(display, XkbUseCoreKbd, XkbStateNotify,
      XkbAllStateComponentsMask, XkbGroupStateMask);
    //// keyboard mapping change:
    XkbSelectEventDetails(display, XkbUseCoreKbd, XkbMapNotify,
      XkbAllMapComponentsMask, XkbKeySymsMask);
    //// group names change:
    XkbSelectEventDetails(display, XkbUseCoreKbd, XkbNamesNotify,
      XkbAllNamesMask, XkbGroupNamesMask);
    //// new keyboard:
    XkbSelectEventDetails(display, XkbUseCoreKbd, XkbNewKeyboardNotify,
      XkbAllNewKeyboardEventsMask, XkbAllNewKeyboardEventsMask);
    // retrieve the number of keyboard groups
    retrieveNumKbdGroups();
  }
  m_self = this;

}

XKeyboard::~XKeyboard(){
 qDebug()<<"XKeyboard destructor";
}

/** Set the current keyboard group to the given groupno */
void XKeyboard::setGroupNo(int groupno){
  XkbLockGroup(QX11Info::display(), XkbUseCoreKbd, groupno);
}

#ifndef HAVE_LIBXKLAVIER
extern "C" {
  static int IgnoreXError(Display *, XErrorEvent *) {
    return 0;
  }
}
#endif

XKeyboard * XKeyboard::self()
{
  return m_self;
}

/** return the current keyboard group index */
int XKeyboard::getGroupNo(){
  XkbStateRec rec;
  XkbGetState(QX11Info::display(), XkbUseCoreKbd, &rec);
  return (int) rec.group;
}



void XKeyboard::retrieveNumKbdGroups(){
  XkbDescRec xkb;

  memset(&xkb, 0, sizeof(xkb));
  /* Interestingly, in RedHat 6.0 (XFree86 3.3.3.1) the XkbGetControls call
  below works even if xkb.device_spec is not set. But in RedHat 7.1 (XFree86 4.0.3)
  it returns BadImplementation status code, and you have to specify
  xkb.device_spec = XkbUseCoreKbd. */
  xkb.device_spec = XkbUseCoreKbd;
  XkbGetControls(QX11Info::display(), XkbGroupsWrapMask, &xkb);
  m_numgroups = xkb.ctrls->num_groups;
  XkbFreeControls(&xkb, XkbGroupsWrapMask, 1);
}


/** Examines an X Event passed to it and takes actions if the event is of
  * interest to XKeyboard */
void XKeyboard::processEvent(XEvent *ev) {
  if (ev->type == m_event_code) {
    // This an XKB event
    XkbEvent *xkb_ev = (XkbEvent *) ev;
    if (xkb_ev->any.xkb_type == XkbStateNotify) {
      // state notify event, the current group has changed
      emit groupChanged(xkb_ev->state.group);
    }
    else if (((xkb_ev->any.xkb_type == XkbMapNotify) && (xkb_ev->map.changed & XkbKeySymsMask))
      || ((xkb_ev->any.xkb_type == XkbNamesNotify) && (xkb_ev->names.changed & XkbGroupNamesMask))
      || (xkb_ev->any.xkb_type == XkbNewKeyboardNotify)) {
      // keyboard layout has changed
      retrieveNumKbdGroups();
      emit layoutChanged();      
    }

   }
}

