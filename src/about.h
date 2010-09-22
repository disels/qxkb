////////////////////////////////////////
//  File      : about.h               //
//  Written by: disels@gmail.com      //
//  Copyright : GPL                   //
////////////////////////////////////////

#ifndef ABOUT_H
#define ABOUT_H

#include "defs.h"
#include "ui_about.h"

class About : public QDialog
{
    Q_OBJECT
public:
    About(QWidget *parent=0);
   void about(QString title,QString text);
    void aboutQt(QString title);

protected:
     void closeEvent(QCloseEvent *event);

private:
     Ui_about ui;
};

#endif // ABOUT_H
