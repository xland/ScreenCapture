#pragma once
#include <QTextEdit>
class TextInputBox : public QTextEdit
{
	Q_OBJECT
public:
	explicit TextInputBox(QWidget* parent = nullptr);
	~TextInputBox();
	void setFontFamilyCustom(const QString& name);
	void setFontSizeCustom(const int& size);
	void setColorCustom(const QColor& color);
signals:
	void doubleClicked(QMouseEvent* e);
protected:
	void mouseDoubleClickEvent(QMouseEvent* e) override;
private:
	void resizeInputToContent();
};

