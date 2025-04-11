#include <QApplication>

#include "Lang.h"

Lang* lang;

Lang::Lang(QObject *parent) : QObject(parent)
{
	
}

Lang::~Lang()
{

}

QString Lang::get(const QString& name)
{
	return lang->dic[name];
}

void Lang::init(const QString& langType)
{
	lang = new Lang(qApp);
	if (langType == "ZhCn") {
		lang->initZhCn();
	}
	else if (langType == "En") {
		lang->initEn();
	}
}

void Lang::initZhCn()
{
	dic.insert("toolBar", "工具栏（Ctrl+T）");
	dic.insert("quit", "退出（Esc）");

	dic.insert("rect", "矩形");
	dic.insert("ellipse", "圆形");
	dic.insert("arrow", "箭头");
	dic.insert("number", "标号");
	dic.insert("line", "线条");
	dic.insert("text", "文本");
	dic.insert("mosaic", "");
	dic.insert("eraser", "");
	dic.insert("undo", "");
	dic.insert("redo", "");
	dic.insert("pin", "");
	dic.insert("clipboard", "");
	dic.insert("save", "");
	dic.insert("close", "");
}

void Lang::initEn()
{
	dic.insert("toolBar", "Toolbar (Ctrl+T)");
	dic.insert("quit", "Quit (Esc)");

	dic.insert("rect", "Rect");
	dic.insert("ellipse", "Ellipse");
	dic.insert("arrow", "Arrow");
	dic.insert("number", "Number");
	dic.insert("line", "Pen");
	dic.insert("text", "Text");
	dic.insert("mosaic", "");
	dic.insert("eraser", "");
	dic.insert("undo", "");
	dic.insert("redo", "");
	dic.insert("pin", "");
	dic.insert("clipboard", "");
	dic.insert("save", "");
	dic.insert("close", "");


}
