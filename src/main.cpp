
#include "MainWindow.h"
#include "Icon.h"
#include <QApplication>
#include <QTextStream>
#include <QMutex>
#include <qdebug.h>


void logToFile(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    static QMutex mutex;
    mutex.lock();
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date = QString("(%1)").arg(current_date_time);
    QString text;
    switch (type)
    {
    case QtDebugMsg:
        text = current_date + QString("Debug:");
        break;

    case QtWarningMsg:
        text = current_date + QString("Warning:");
        break;

    case QtCriticalMsg:
        text = current_date + QString("Critical:");
        break;

    case QtFatalMsg:
        text = current_date + QString("Fatal:");
        break;
    case QtInfoMsg:
        text = current_date + QString("InfoMsg:");
        break;
    }
    QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
    QString message = QString("%1 %2 %3 %4").arg(text).arg(context_info).arg(msg);
    QFile file("ScreenCapture.log");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream textStream(&file);
    textStream << message << "\r\n";
    file.flush();
    file.close();
    mutex.unlock();
}



int main(int argc, char* argv[])
{
    qInstallMessageHandler(logToFile);
    qDebug()<<"start";
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QApplication a(argc, argv);
    Icon::Init();
    MainWindow w;
    return a.exec();
}
