#include <QDateTime>

#include "../Win/WinBase.h"
#include "../Tool/ToolSub.h"
#include "../Shape/ShapeBase.h"
#include "../Shape/ShapeRect.h"
#include "../Shape/ShapeEllipse.h"
#include "../Shape/ShapeArrow.h"
#include "../Shape/ShapeNumber.h"
#include "../Shape/ShapeLine.h"
#include "../Shape/ShapeText.h"
#include "../Shape/ShapeEraserRect.h"
#include "../Shape/ShapeEraserLine.h"
#include "../Shape/ShapeMosaicRect.h"
#include "../Shape/ShapeMosaicLine.h"
#include "Canvas.h"
#include "../App/Util.h"


Canvas::Canvas(QObject *parent) : QObject(parent)
{
	auto win = (WinBase*)parent;
    auto dpr = win->devicePixelRatio();
    imgBg = Util::printScreen();
    imgBg.setDevicePixelRatio(dpr);
    imgBoard = imgBg.copy();
    imgCanvas = imgBg.copy();
}

Canvas::~Canvas()
{

}

void Canvas::undo()
{
	if (shapes.isEmpty()) return;	
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        if (shapes[i]->state != ShapeState::undo) {
			shapes[i]->state = ShapeState::undo;
			break;
        }
    }
	imgBoard.fill(Qt::transparent);
	QPainter p(&imgBoard);
	p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
	for (auto& s : shapes)
	{
		if (s->state == ShapeState::undo) continue;
		s->paint(&p);
	}
	auto win = (WinBase*)parent();
	win->update();
}

void Canvas::redo()
{
    if (shapes.isEmpty()) return;
    for (int i = 0; i < shapes.size(); i++)
    {
        if (shapes[i]->state == ShapeState::undo) {
            shapes[i]->state = ShapeState::ready;
            break;
        }
    }
    imgBoard.fill(Qt::transparent);
    QPainter p(&imgBoard);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    for (auto& s : shapes)
    {
        if (s->state == ShapeState::undo) continue;
        s->paint(&p);
    }
    auto win = (WinBase*)parent();
    win->update();
}

void Canvas::mousePress(QMouseEvent* event)
{
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        auto flag = shapes[i]->mousePress(event);
        if (flag) {
            return;
        }
    }
    addShape();
    shapeCur->mousePress(event);
}

void Canvas::mouseDrag(QMouseEvent* event)
{
    if (shapeCur) {
        shapeCur->mouseDrag(event);
    }
}

void Canvas::mouseMove(QMouseEvent* event)
{
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        auto flag = shapes[i]->mouseMove(event);
        if (flag) {
            return;
        }
    }
}

void Canvas::mouseRelease(QMouseEvent* event)
{
    if (shapeCur) {
        auto flag = shapeCur->mouseRelease(event);
        if (flag) {
            auto win = (WinBase*)parent();
            if (win->state == State::text) {
                return;
            }
            paintShapeOnBoard(shapeCur);
        }
        else {
            removeShapeCur();
        }
        shapeCur = nullptr;
    }
}

void Canvas::paint(QPainter& p)
{
    p.drawImage(0, 0, imgBg);
    p.drawImage(0, 0, imgBoard);
    if (shapeCur) {
        shapeCur->paint(&p);
    }
    if (shapeHover) {
        shapeHover->paintDragger(&p);
    }
}

void Canvas::addShape()
{
    auto win = (WinBase*)parent();
    if (win->state == State::rect) {
        shapeCur = new ShapeRect(win);
    }
    else if (win->state == State::ellipse) {
        shapeCur = new ShapeEllipse(win);
    }
    else if (win->state == State::arrow) {
        shapeCur = new ShapeArrow(win);
    }
    else if (win->state == State::number) {
        shapeCur = new ShapeNumber(win);
    }
    else if (win->state == State::line) {
        shapeCur = new ShapeLine(win);
    }
    else if (win->state == State::text) {
        shapeCur = new ShapeText(win);
    }
    else if (win->state == State::mosaic) {
        auto isRect = win->toolSub->getSelectState("mosaicFill");
        if (isRect) {
            shapeCur = new ShapeMosaicRect(win);
        }
        else {
            shapeCur = new ShapeMosaicLine(win);
        }
    }
    else if (win->state == State::eraser) {
        auto isRect = win->toolSub->getSelectState("eraserFill");
        if (isRect) {
            shapeCur = new ShapeEraserRect(win);
        }
        else {
            shapeCur = new ShapeEraserLine(win);
        }
    }
    else
    {
        return;
    }
    
}

void Canvas::setHoverShape(ShapeBase* shape)
{
    if (!timerDragger) {
        timerDragger = new QTimer(this);
        timerDragger->setInterval(800);
        timerDragger->setSingleShot(true);
        connect(timerDragger, &QTimer::timeout, [this]() {
            if (shapeHover->hoverDraggerIndex != -1) {
                timerDragger->start();
            }
            else {
                shapeHover = nullptr;
                auto win = (WinBase*)parent();
                win->update();
            }
            });
    }
    if (shape != shapeHover) {
        shapeHover = shape;
    }
    auto win = (WinBase*)parent();
    win->update();
    timerDragger->start();
}

void Canvas::removeShapeFromBoard(ShapeBase* shape)
{
    auto win = (WinBase*)parent();
    imgBoard.fill(Qt::transparent);
    QPainter p(&imgBoard);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    for (auto& s : shapes)
    {
        if (shape == s) continue;
        s->paint(&p);
    }
    shapeCur = shape; //这行主要是为了显示Dragger
}

void Canvas::paintShapeOnBoard(ShapeBase* shape)
{
    auto win = (WinBase*)parent();
    QPainter p(&imgBoard);
    p.setRenderHint(QPainter::Antialiasing, true);
    shape->paint(&p);
    win->update();
	if (!shapes.contains(shape)) {
		shapes.push_back(shape); //防止文本对象重复添加
	}
}

void Canvas::removeShapeCur()
{
    shapes.removeOne(shapeCur);
    shapeCur->deleteLater();
    shapeCur = nullptr;
}
