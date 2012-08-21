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

QString getName(QString pidExist)
{
    qDebug()<<"Find /proc/"<<pidExist;
    QFile status("/proc/"+pidExist+"/status");
    qDebug()<<"File /proc/"+pidExist+"/status";
    if(!status.exists())
    {
        qDebug()<<"File /proc/"+pidExist+"/status dose not exist";
        return "";
    }
    if (!status.open(QIODevice::ReadOnly | QIODevice::Text))
    {
         qDebug()<<"Can`t open /proc/"+pidExist+"/status";
         return "";
    }
    qDebug()<<"Open status file";
    // Read the first line from the file /proc/ <pid>/ status, unframed and calculate file size.
    // Probably not the best idea
    QTextStream in(&status);
    QString procName = in.readLine();
    qDebug()<<procName;
    if (procName.contains("Name:"))
    {
       procName=procName.split(":").at(1).trimmed();
       qDebug()<<"Get proc name"<<procName;
       status.close();
       qDebug()<<"Poccess name: "<<procName;
         return procName;
     }
    return "";
}

bool isRun(qint64 pid)
{
    QString pidPath(QDir::homePath());
    pidPath=pidPath+"/.config/qxkb.pid";
    qDebug()<<"Pid file locate :"<<pidPath;
    QFile pidFile(pidPath);
    QDir proc(QString("/proc"));
    if (pidFile.exists())
    {
        qDebug()<<"File exist check pid";
       if (!pidFile.open(QIODevice::ReadOnly | QIODevice::Text))
       {
           qDebug()<<"Faled open file";
           return false;
       }
       QStringList proccessList=proc.entryList(QDir::Dirs);
        qDebug()<<"Proc list "<<proccessList;
       QTextStream in(&pidFile);
        while (!in.atEnd()) {
            QString pidString = in.readLine();
            qDebug()<<"Get exist pid"<<pidString;
            int index = proccessList.indexOf(pidString.trimmed());
            qDebug()<<"Index pid in list"<<index;
            if (index<0)
            {
                pidFile.close();
                pidFile.remove();
            }
            else
            {   QString pidExist=proccessList.at(index);
                qDebug()<<"Compare processname: "<<pidExist;
                if (getName(pidExist)=="qxkb")
                {
                    pidFile.close();
                    return true;
                }
            }

        }
        pidFile.close();
        pidFile.remove();
    }
    if (!pidFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug()<<"Faled to create pid";
       return false;
    }
    qDebug()<<"Create pid";
    QTextStream out(&pidFile);
    out << QString::number(pid);
    pidFile.close();
    return false;
}

int main(int argc, char *argv[])
{
    //qInstallMsgHandler(myMessageOutput);
    QXKB a(argc, argv);

    QTranslator translator;
    qDebug() << "Locale:" << QLocale::system().name();
    QString lang = "qxkb_" + QLocale::system().name().split("_")[0];
    qDebug()<<"Language:"<<lang;
    QString langPath(QXKB_TRANSLATION_PATH);
    qDebug()<<"Language path (trying):"<<langPath;
    if (!QFile::exists(langPath+"/"+lang+".qm"))
        langPath =  QCoreApplication::applicationDirPath() + "/language/";
    qDebug()<<"Language path (fallback):"<<langPath;
    translator.load(lang,langPath);
    a.installTranslator(&translator);
    a.setStartup();
    qint64 pid = a.applicationPid();
    if (isRun(pid))
    {
        qDebug()<<"One copy of qxkb run in current user session";
        return 0;
    }
    return a.exec();
}
