#include "mainwindow.h"
#include <QApplication>
#include <QMutex>
#include <QDateTime>
#include <QDir>
#include <QDebug>

MainWindow *pw = nullptr;
QMutex mutex;

void HPM100MessageOutput(QtMsgType type, const QMessageLogContext & context, const QString &msg)
{
    Q_UNUSED(context);
    //创建log文件夹
    QDir dir("log");
    if (!dir.exists())
    {
         QDir dir;
         dir.mkdir("log");
    }

    //创建log文件
    QString currentDate = QDateTime::currentDateTime().toString("yyyyMMdd");
    QString logName = "log" + currentDate + ".txt";
    QString logFileName = "log/" + logName;

    //加锁
    mutex.lock();

    //QByteArray localMsg = msg.toLocal8Bit();

    //信息分类
    QString strMsg("");

    switch (type)
    {
    case QtDebugMsg:
        strMsg = QString("[Communication]");
        break;
    case QtWarningMsg:
        strMsg = QString("[Info]");
        break;
    case QtCriticalMsg:
        strMsg = QString("[Critical]");
        break;
    case QtFatalMsg:
        strMsg = QString("[Error]");
        break;
    default:
        break;
    }

     //在.pro文件定义以下的宏，可以屏蔽相应的日志输出
     //DEFINES += QT_NO_WARNING_OUTPUT
     //DEFINES += QT_NO_DEBUG_OUTPUT
     //DEFINES += QT_NO_INFO_OUTPUT

    //文件名、函数名、行数
    //strMsg += QString("Function: %1  File: %2  Line: %3 ").arg(context.function).arg(context.file).arg(context.line);

    //设置输出信息格式
    QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz");
    QString strMessage = QString("%1 %2 :%3").arg(strDateTime).arg(strMsg).arg(msg);

    if(pw)
    {
        //输出日志到UI
        pw->appendMsgout(strMessage);
    }

    //写入文件
    QFile file(logFileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        file.close();
        return ;
    }
    QTextStream stream(&file);
    stream << strMessage << "\r\n";;
    file.flush();
    file.close();

    //解锁
    mutex.unlock();
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qInstallMessageHandler(HPM100MessageOutput);


    int exitcode = 0;
    do
    {
        MainWindow w;
        pw = &w;
        w.show();
        exitcode = a.exec();
    }
    while(exitcode == MainWindow::EXIT_CODE_REBOOT);

    return exitcode;
}
