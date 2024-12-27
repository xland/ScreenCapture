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
	void undo();
	void redo();
	void keyEscPress() override;
	void hideTools(State state=State::start);
public:
	State state{ State::start };
	QList<ShapeBase*> shapes;
	ToolMain* toolMain;
	ToolSub* toolSub;
	PixelInfo* pixelInfo;
	WinCanvas* winCanvas;
	WinBoard* winBoard;
protected:
	ShapeBase* addShape();
	void mousePressOnShape(QMouseEvent* event);
	void mouseMoveOnShape(QMouseEvent* event);
	void mouseDragOnShape(QMouseEvent* event);
	void mouseReleaseOnShape(QMouseEvent* event);
	void removeShape() override;
	void mouseDBClick(QMouseEvent* event) override;
	void mousePressRight(QMouseEvent* event) override;
	void copyColor(const int& key) override;
private:
};
