#include <QApplication>
#include <QClipboard>
#include <QTimer>
#include <QMimeData>
#include <QFileInfo>
#include <iostream>
#include <fcntl.h>
#include <Windows.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include "App.h"
#include "Lang.h"
#include "Util.h"
#include "Win/WinFull.h"
#include "Win/WinPin.h"

namespace {
    std::unique_ptr<App> app;
    QString defaultSavePath;
    int compressSize{ 100 };
    int compressQuality{ -1 };
	int customCap{ -1 };
    QStringList tools;
}
QMap<QString, QString> App::getCmd()
{
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
    return params;
}
bool App::parseCmd(const QMap<QString, QString>& params) {
    if (params.contains("comp")) {
		if(!setCompressVal(params["comp"])) return false;
    }
    if (params.contains("path")) {
        defaultSavePath = params["path"];
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
	if (params.contains("cap")) {
		auto cap = params["cap"];
        if (cap.startsWith("area")) {
            capArea(cap);
            return true;
        }
        else if (cap.startsWith("fullscreen")) {
            capFullscreen(cap);
            return true;
        }
        //else if (cap.startsWith("long")) {
        //    new WinLong();
        //    return true;
        //}
        else if (cap.startsWith("custom")) {
            capCustom(cap);
            return false;
        }
	}
    if (params.contains("tool")) {
        tools = params["tool"].split(",");
    }
    return false;
}
void App::pinClipboard(const QString& cmd)
{
    auto arr = cmd.split(",");
    int x{ 100 }, y{100};
	bool ok;
    if (arr.size() == 3) {
        x = arr[1].toInt(&ok);
        if (!ok) {
            qDebug() << "pin clipboard param error.";
            exit(10);
            return;
        }
        y = arr[2].toInt(&ok);
        if (!ok) {
            qDebug() << "pin clipboard param error.";
            exit(10);
            return;
        }
    }
    QClipboard* clipboard = QApplication::clipboard();
    const QMimeData* mimeData = clipboard->mimeData();
    QImage img;
    if (mimeData->hasImage()) {
        img = clipboard->image();
    }
    else if (mimeData->hasUrls()) {
        auto url = mimeData->urls().first();
        QString filePath = url.toLocalFile();
		if (Util::isImagePath(filePath)) {
			img.load(filePath);
		}
    }
    else if (mimeData->hasText()) {
        QString filePath = clipboard->text();
        if (Util::isImagePath(filePath)) {
            img.load(filePath);
        }
    }
	if (img.isNull()) {
        qDebug() << "No image in clipboard.";
        exit(10);
		return;
	}
    new WinPin(QPoint(x,y), img);
}
void App::pinFile(const QString& cmd)
{
    auto arr = cmd.split(",");
	if (arr.size() < 2) {
		qDebug() << "pin file param error.";
		exit(10);
		return;
	}
    QImage img(arr[1]);
    if (img.isNull()) {
        qDebug() << "Image Path error.";
        exit(10);
        return;
    }
    int x{ 100 }, y{ 100 };
    bool ok;
    if (arr.size() == 4) {
        x = arr[2].toInt(&ok);
        if (!ok) {
            qDebug() << "pin clipboard param error.";
            exit(10);
            return;
        }
        y = arr[3].toInt(&ok);
        if (!ok) {
            qDebug() << "pin clipboard param error.";
            exit(10);
            return;
        }
    }
    new WinPin(QPoint(x, y), img);
}
void App::pinArea(const QString& cmd)
{
    auto arr = cmd.split(",");
	if (arr.size() < 5) {
		qDebug() << "pin area param error.";
		exit(10);
        return;
	}
    bool ok;
	auto x = arr[1].toInt(&ok);
	if (!ok) {
		qDebug() << "pin area param error.";
		exit(10);
        return;
	}
	auto y = arr[2].toInt(&ok);
	if (!ok) {
		qDebug() << "pin area param error.";
		exit(10);
        return;
	}
	auto w = arr[3].toInt(&ok);
	if (!ok) {
		qDebug() << "pin area param error.";
		exit(10);
        return;
	}
	auto h = arr[4].toInt(&ok);
	if (!ok) {
		qDebug() << "pin area param error.";
		exit(10);
        return;
	}
    int x1{ 100 }, y1{ 100 };
    if (arr.size() == 7) {
        x1 = arr[5].toInt(&ok);
        if (!ok) {
            qDebug() << "pin area param error.";
            exit(10);
            return;
        }
        y1 = arr[6].toInt(&ok);
        if (!ok) {
            qDebug() << "pin area param error.";
            exit(10);
            return;
        }
    }
	QImage img = Util::printScreen(x, y, w, h);
	new WinPin(QPoint(x1, y1),img);
}
bool App::setCompressVal(const QString& cmd)
{
    auto arr = cmd.split(",");
    bool ok;
    if (arr.size() >= 1) {
		compressQuality = arr[0].toInt(&ok);
		if (!ok) {
			qDebug() << "comp param error.";
			exit(10);
			return false;
		}
    }
    if (arr.size() >= 2) {
		compressSize = arr[1].toInt(&ok);
		if (!ok) {
			qDebug() << "comp param error.";
			exit(10);
			return false;
		}
    }
    return true;
}
void App::capArea(const QString& cmd)
{
	auto arr = cmd.split(",");
	if (arr.size() < 5) {
		qDebug() << "cap area param error.";
		exit(10);
		return;
	}
	bool ok;
	auto x = arr[1].toInt(&ok);
	if (!ok) {
		qDebug() << "cap area param error.";
		exit(10);
		return;
	}
	auto y = arr[2].toInt(&ok);
	if (!ok) {
		qDebug() << "cap area param error.";
		exit(10);
		return;
	}
	auto w = arr[3].toInt(&ok);
	if (!ok) {
		qDebug() << "cap area param error.";
		exit(10);
		return;
	}
	auto h = arr[4].toInt(&ok);
	if (!ok) {
		qDebug() << "cap area param error.";
		exit(10);
		return;
	}
	QImage img = Util::printScreen(x, y, w, h);
	if (arr.size() == 6) {
        if (arr[5] == "clipboard") {
            Util::imgToClipboard(img);
            exit(9);
        }
	}
    else
    {
		Util::saveToFile(img);
        exit(8);
    }	
}
void App::capFullscreen(const QString& cmd)
{
    auto x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    auto y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    auto w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    auto h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    auto img = Util::printScreen(x, y, w, h);
    auto arr = cmd.split(",");
    if (arr.size() == 2) {
        if (arr[1] == "clipboard") {
            Util::imgToClipboard(img);
            exit(9);
        }
    }
    else
    {
        Util::saveToFile(img);
        exit(8);
    }
}
bool App::capCustom(const QString& cmd)
{
    auto arr = cmd.split(",");
    if (arr.size() == 2) {
        if (arr[1] == "clipboard") {
            customCap = 1;
        }
    }
    else
    {
        customCap = 0;
    }
    return true;
}
void App::exit(const int& code)
{
	QTimer::singleShot(10, [code]() {
		qApp->exit(code);
		});
}
void App::attachConsole()
{
    if (::AttachConsole(ATTACH_PARENT_PROCESS)) {
        FILE* unused;
        if (freopen_s(&unused, "CONOUT$", "w", stdout)) {
            _dup2(_fileno(stdout), 1);
        }
        if (freopen_s(&unused, "CONOUT$", "w", stderr)) {
            _dup2(_fileno(stdout), 2);
        }
        std::ios::sync_with_stdio();
        qDebug() << "Hello ScreenCapture";
    }
}
void App::init()
{
    QFont font("Microsoft YaHei", 9);
    qApp->setFont(font);
	auto cmds = getCmd();
    if (cmds.size() > 0) {
        attachConsole();
    }
    auto flag = parseCmd(cmds);
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

std::tuple<int, int> App::getCompressVal()
{
    return {compressQuality,compressSize};
}

int App::getCustomCap()
{
    return customCap;
}

QStringList App::getTool()
{
    return tools;
}
