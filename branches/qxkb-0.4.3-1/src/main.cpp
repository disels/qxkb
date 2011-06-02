#include "defs.h"
#include "qxkb.h"


void myMessageOutput(QtMsgType type, const char *msg)
 {
     switch (type) {
     case QtDebugMsg:
         fprintf(stderr, "Debug: %s\n", msg);
         break;
     case QtWarningMsg:
         fprintf(stderr, "Warning: %s\n", msg);
         break;
     case QtCriticalMsg:
         fprintf(stderr, "Critical: %s\n", msg);
         break;
     case QtFatalMsg:
         fprintf(stderr, "Fatal: %s\n", msg);
         abort();
     }
 }

int main(int argc, char *argv[])
{
    //qInstallMsgHandler(myMessageOutput);
    QXKB a(argc, argv);

    QTranslator translator;
    qDebug() << "Locale:" << QLocale::system().name();
    QString lang = "qxkb_" + QLocale::system().name().split("_")[0];
    qDebug()<<"Language:"<<lang;
    QString langPath=QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    if (!QFile::exists(langPath+"/"+lang+".qm"))
        langPath =  QCoreApplication::applicationDirPath() + "/language/";
    qDebug()<<"Language path:"<<langPath;
    translator.load(lang,langPath);
    a.installTranslator(&translator);
    a.setStartup();
    return a.exec();
}
