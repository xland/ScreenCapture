#include "TextInputBox.h"
#include "qdebug.h"

TextInputBox::TextInputBox(QWidget* parent):QTextEdit(parent)
{
	auto font = this->font();
	font.setFamily("Microsoft YaHei");
	setFont(font);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	setStyleSheet("background:transparent;border:1px solid #000;color:red;");
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setFrameShape(QFrame::NoFrame);
	setFrameShadow(QFrame::Plain);
	setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
	setLineWrapMode(QTextEdit::NoWrap);
	setAcceptRichText(false);
	connect(this, &QTextEdit::textChanged, this, &TextInputBox::resizeInputToContent);
	setHtml(R"(<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd">
<html><head><meta name="qrichtext" content="1" /><meta charset="utf-8" /><style type="text/css">
p, li { white-space: pre-wrap; }
hr { height: 1px; border-width: 0; }
li.unchecked::marker { content: "\2610"; }
li.checked::marker { content: "\2612"; }
</style></head><body style=" font-family:'Microsoft YaHei UI'; font-size:28pt; font-weight:400; font-style:normal;">
<p style="-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;"><br /></p></body></html>)");
}
TextInputBox::~TextInputBox()
{

}
void TextInputBox::mouseDoubleClickEvent(QMouseEvent* e)
{
	emit doubleClicked(e);
}

void TextInputBox::setFontFamilyCustom(const QString& name)
{
	if (name == "微软雅黑")
	{
		auto font = this->font();
		font.setFamily("Microsoft YaHei");
		this->setFont(font);
	}
	else
	{
		auto font = this->font();
		font.setFamily("SimSun");
		this->setFont(font);
	}
	this->resizeInputToContent();
}
void TextInputBox::setFontSizeCustom(const int& size)
{
	auto font = this->font();
	font.setPointSize(size + 20);
	this->setFont(font);
	this->resizeInputToContent();
}
void TextInputBox::setColorCustom(const QColor& color)
{
	QString style = QString("background:transparent;border:1px solid #000;color:rgb(%1,%2,%3);");
	setStyleSheet(style.arg(QString::number(color.red()), QString::number(color.green()), QString::number(color.blue())));
}
void TextInputBox::resizeInputToContent()
{
	this->document()->adjustSize();
	auto size = this->document()->size().toSize();
	this->resize(size.width() + 6, size.height() + 6);
}