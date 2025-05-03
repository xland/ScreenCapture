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
	if (langType == "zhcn") {
		lang->initZhCn();
	}
	else if (langType == "en") {
		lang->initEn();
	}
}

void Lang::initZhCn()
{
	dic.insert("saveFile", "保存文件");

	dic.insert("rect", "矩形");
	dic.insert("ellipse", "圆形");
	dic.insert("arrow", "箭头");
	dic.insert("number", "标号");
	dic.insert("line", "线条");
	dic.insert("text", "文本");
	dic.insert("mosaic", "马赛克");
	dic.insert("eraser", "橡皮擦");
	dic.insert("undo", "撤销");
	dic.insert("redo", "重做");
	dic.insert("pin", "钉住");
	dic.insert("clipboard", "保存到剪切板");
	dic.insert("save", "保存");
	dic.insert("close", "关闭");

	dic.insert("rectFill", "填充矩形");
	dic.insert("strokeCtrl", "线条粗细：");
	dic.insert("ellipseFill", "填充椭圆");
	dic.insert("arrowFill", "填充箭头");
	dic.insert("numberFill", "填充标号");
	dic.insert("lineTransparent", "半透明线条");
	dic.insert("bold", "粗体");
	dic.insert("italic", "斜体");
	dic.insert("mosaicFill", "矩形马赛克");
	dic.insert("eraserFill", "矩形橡皮擦");

	dic.insert("red", "红");
	dic.insert("yellow", "黄");
	dic.insert("green", "绿");
	dic.insert("cyan", "青");
	dic.insert("blue", "蓝");
	dic.insert("purple", "紫");
	dic.insert("pink", "粉");
	dic.insert("black", "黑");
}

void Lang::initEn()
{
	dic.insert("saveFile", "Save File");

	dic.insert("rect", "Rect");
	dic.insert("ellipse", "Ellipse");
	dic.insert("arrow", "Arrow");
	dic.insert("number", "Number");
	dic.insert("line", "Pen");
	dic.insert("text", "Text");
	dic.insert("mosaic", "Mosaic");
	dic.insert("eraser", "Eraser");
	dic.insert("undo", "Undo");
	dic.insert("redo", "Redo");
	dic.insert("pin", "Pin");
	dic.insert("clipboard", "Save to Clipboard");
	dic.insert("save", "Save to Disk");
	dic.insert("close", "Close");

	dic.insert("rectFill", "Filled Rect");
	dic.insert("strokeCtrl", "Stroke Width: ");
	dic.insert("ellipseFill", "Filled Ellipse");
	dic.insert("arrowFill", "Filled Arrow");
	dic.insert("numberFill", "Filled Number");
	dic.insert("lineTransparent", "Translucent Line");
	dic.insert("bold", "Bold");
	dic.insert("italic", "Italics");
	dic.insert("mosaicFill", "Rectangle Mosaic");
	dic.insert("eraserFill", "Rectangle Eraser");


	dic.insert("red", "Red");
	dic.insert("yellow", "Yellow");
	dic.insert("green", "Green");
	dic.insert("cyan", "Cyan");
	dic.insert("blue", "Blue");
	dic.insert("purple", "Purple");
	dic.insert("pink", "Pink");
	dic.insert("black", "Black");
}
