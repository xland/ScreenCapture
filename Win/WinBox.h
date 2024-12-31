#pragma once

#include <QWidget>
#include "WinBase.h"

class ToolMain;
class ToolSub;
class PixelInfo;
class WinBoard;
class WinCanvas;
class ShapeBase;
class WinBox : public WinBase
{
	Q_OBJECT

public:
	WinBox(QObject* parent = nullptr);
	virtual ~WinBox();
	virtual void showToolMain()=0;
	virtual void showToolSub()=0;
	virtual void saveToClipboard() = 0;
	virtual void saveToFile() = 0;
	virtual void copyColor(const int& key) = 0;
	virtual void moveByKey(const int& key) = 0;
	void removeShape();
	void undo();
	void redo();
	void keyEscPress();
	void hideTools(State state=State::start);
public:
	State state{ State::start };
	QList<ShapeBase*> shapes;
	ToolMain* toolMain;
	ToolSub* toolSub;
	PixelInfo* pixelInfo;
	WinCanvas* winCanvas;
	WinBoard* winBoard;
	qint16 padding{ 0 };
protected:
	ShapeBase* addShape();
	void mousePressOnShape(QMouseEvent* event);
	void mouseMoveOnShape(QMouseEvent* event);
	void mouseDragOnShape(QMouseEvent* event);
	void mouseReleaseOnShape(QMouseEvent* event);
	bool processOtherMsg(UINT msg, WPARAM wParam, LPARAM lParam) override;
	virtual void ctrlTPress() {};
private:
};
