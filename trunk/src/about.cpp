////////////////////////////////////////
//  File      : about.cpp             //
//  Written by: disels@gmail.com      //
//  Copyright : GPL                   //
////////////////////////////////////////

#include "about.h"

About::About(QWidget *parent): QDialog(parent)
{
  ui.setupUi(this);
}

void About::closeEvent(QCloseEvent *event)
 {
     qDebug()<<"About:Close event";
     hide();
     event->ignore();
 }

void About::about(QString title,QString text)
{
 setWindowTitle(title);
 ui.image->setPixmap(QPixmap(":/about/qxkb.png"));
 ui.label->setText (text);
}

void About::aboutQt(QString title)
{
    setWindowTitle(title.isEmpty() ? tr("About Qt") : title);
    QString translatedTextAboutQt;
    translatedTextAboutQt = tr(
        "<h3>About Qt</h3>"
        "%1<p>Qt is a C++ toolkit for cross-platform <br>"
        "application development.</p>"
        "<p>Qt provides single-source "
        "portability across MS&nbsp;Windows, Mac&nbsp;OS&nbsp;X, <br>"
        "Linux, and all major commercial Unix variants. Qt is also <br>"
        " available for embedded devices as Qt for Embedded Linux<br>"
        " and Qt for Windows CE.</p>"
        "<p>Qt is a Nokia product. See <br>"
        "<a href=\"http://qtsoftware.com/qt/\">qtsoftware.com/qt/</a> for more information.</p>"
       )
#if QT_EDITION != QT_EDITION_OPENSOURCE
       .arg(QMessageBox::tr("<p>This program uses Qt version %1.</p>"))
#else
       .arg(QMessageBox::tr("<p>This program uses Qt Open Source Edition version %1.</p>"
               "<p>Qt Open Source Edition is intended for the development<br> "
               "of Open Source applications. You need a commercial Qt <br>"
               "license for development of proprietary (closed source)<br> "
               "applications.</p>"
               "<p>Please see <a href=\"http://qtsoftware.com/company/model/\">qtsoftware.com/company/model/</a> "
               "for an overview of Qt licensing.</p>"))
#endif

    .arg(QLatin1String(QT_VERSION_STR));

     ui.label->setText (translatedTextAboutQt);
     ui.image->setPixmap(QPixmap(":/about/qt.png"));
}
