#include <QApplication>
#include <QClipboard>
#include "App.h"
#include "Lang.h"
#include "Util.h"
#include "Win/WinFull.h"
#include "Win/WinPin.h"

namespace {
    std::unique_ptr<App> app;
    QString defaultSavePath;
}
bool App::parseCmd() {
    QStringList args = QCoreApplication::arguments();
    QMap<QString, QString> params;
    for (int i = 1; i < args.size(); ++i) {
        QString arg = args[i].trimmed();
        if (!arg.startsWith("--")) {
            continue;
        }
        arg = arg.mid(2);
        int colonIndex = arg.indexOf(':');
        if (colonIndex == -1) {
            continue;
        }
        QString key = arg.left(colonIndex).toLower();
        QString value = arg.mid(colonIndex + 1);
        if (value.startsWith('"') && value.endsWith('"')) {
            value = value.mid(1, value.length() - 2);
        }
        params[key] = value;
    }

    if (params.contains("dir")) {
        defaultSavePath = params["dir"];
    }
    if (params.contains("lang")) {
		QString lang = params["lang"];
		if (lang == "en") {
			Lang::init("en");
		}
		else {
			Lang::init("zhcn");
		}
    }
    else {
		Lang::init("zhcn");
    }
    if (params.contains("pin")) {
		auto pinVal = params["pin"];
        if (pinVal.startsWith("clipboard")) {            
            pinClipboard(pinVal);
            return true;
		}
		else if (pinVal.startsWith("file")) {
			pinFile(pinVal);
            return true;
		}
        else if (pinVal.startsWith("area")) {
            pinArea(pinVal);
            return true;
        }
    }
    return false;
}
void App::pinClipboard(const QString& cmd)
{
    auto arr = cmd.split(",");
    if (arr.size() != 3) {
        qDebug() << "pin clipboard param error.";
        qApp->exit(10);
        return;
    }
	bool ok;
    auto x = arr[1].trimmed().toInt(&ok);
	if (!ok) {
		qDebug() << "pin clipboard param error.";
		qApp->exit(10);
        return;
	}
    auto y = arr[2].trimmed().toInt(&ok);
    if (!ok) {
        qDebug() << "pin clipboard param error.";
        qApp->exit(10);
        return;
    }
    QClipboard* clipboard = QApplication::clipboard();
    QImage img = clipboard->image();
	if (img.isNull()) {
        qDebug() << "No image in clipboard.";
        qApp->exit(10);
		return;
	}
    new WinPin(QPoint(x,y), img);
}
void App::pinFile(const QString& cmd)
{
    auto arr = cmd.split(",");
    if (arr.size() != 4) {
        qDebug() << "pin clipboard param error.";
        qApp->exit(10);
        return;
    }
    bool ok;
    auto x = arr[2].trimmed().toInt(&ok);
    if (!ok) {
        qDebug() << "pin clipboard param error.";
        qApp->exit(10);
        return;
    }
    auto y = arr[3].trimmed().toInt(&ok);
    if (!ok) {
        qDebug() << "pin clipboard param error.";
        qApp->exit(10);
        return;
    }
    QImage img(arr[1]);
    if (img.isNull()) {
        qDebug() << "Image Path error.";
        qApp->exit(10);
        return;
    }
    new WinPin(QPoint(x, y), img);
}
void App::pinArea(const QString& cmd)
{
    auto arr = cmd.split(",");
	if (arr.size() != 7) {
		qDebug() << "pin area param error.";
		qApp->exit(10);
        return;
	}
	bool ok;
	auto x = arr[1].trimmed().toInt(&ok);
	if (!ok) {
		qDebug() << "pin area param error.";
		qApp->exit(10);
        return;
	}
	auto y = arr[2].trimmed().toInt(&ok);
	if (!ok) {
		qDebug() << "pin area param error.";
		qApp->exit(10);
        return;
	}
	auto w = arr[3].trimmed().toInt(&ok);
	if (!ok) {
		qDebug() << "pin area param error.";
		qApp->exit(10);
        return;
	}
	auto h = arr[4].trimmed().toInt(&ok);
	if (!ok) {
		qDebug() << "pin area param error.";
		qApp->exit(10);
        return;
	}
    auto x1 = arr[5].trimmed().toInt(&ok);
    if (!ok) {
        qDebug() << "pin area param error.";
        qApp->exit(10);
        return;
    }
    auto y1 = arr[6].trimmed().toInt(&ok);
    if (!ok) {
        qDebug() << "pin area param error.";
        qApp->exit(10);
        return;
    }
	QImage img = Util::printScreen(x, y, w, h);
	new WinPin(QPoint(x1, y1),img);
}
void App::init()
{
    QFont font("Microsoft YaHei", 9);
    qApp->setFont(font);
    auto flag = parseCmd();
    if (!flag) {
        new WinFull();
    }
}
void App::dispose()
{
    app.reset();       
}

QString App::getSavePath()
{
    return defaultSavePath;
}
