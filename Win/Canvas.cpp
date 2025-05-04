#include <QDateTime>

#include "Canvas.h"
#include "WinBase.h"
#include "CutMask.h"
#include "App/Util.h"
#include "Tool/ToolMain.h"
#include "Tool/ToolSub.h"
#include "Shape/ShapeBase.h"
#include "Shape/ShapeRect.h"
#include "Shape/ShapeEllipse.h"
#include "Shape/ShapeArrow.h"
#include "Shape/ShapeNumber.h"
#include "Shape/ShapeLine.h"
#include "Shape/ShapeText.h"
#include "Shape/ShapeEraserRect.h"
#include "Shape/ShapeEraserLine.h"
#include "Shape/ShapeMosaicRect.h"
#include "Shape/ShapeMosaicLine.h"

Canvas::Canvas(const QImage& img, QObject *parent) : QObject(parent),imgBg{img}
{
	auto win = (WinBase*)parent;    
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
    auto win = (WinBase*)parent();
    if (shapes.first()->state == ShapeState::undo) {
        win->toolMain->setBtnEnable("undo", false);
    }
    win->toolMain->setBtnEnable("redo", true);
	imgBoard.fill(Qt::transparent);
	QPainter p(&imgBoard);
	p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
	for (auto& s : shapes)
	{
		if (s->state == ShapeState::undo) continue;
		s->paint(&p);
	}
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
    auto win = (WinBase*)parent();
    if (shapes.last()->state == ShapeState::ready) {
        win->toolMain->setBtnEnable("redo", false);
    }
    win->toolMain->setBtnEnable("undo", true);
    imgBoard.fill(Qt::transparent);
    QPainter p(&imgBoard);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    for (auto& s : shapes)
    {
        if (s->state == ShapeState::undo) continue;
        s->paint(&p);
    }
    win->update();
}

void Canvas::mousePress(QMouseEvent* event)
{
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        if (shapes[i]->state == ShapeState::undo) continue;
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
    auto flag{ false };
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        if (shapes[i]->state == ShapeState::undo) continue;
        flag = shapes[i]->mouseMove(event);
        if (flag) {
            return;
        }
    }
    if (!flag) {
        auto win = (WinBase*)parent();
        if (win->state == State::text) {
            win->setCursor(Qt::IBeamCursor);
        }
        else {
            win->setCursor(Qt::CrossCursor);
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
	auto win = (WinBase*)parent();
    auto rect = win->rect();
    p.drawImage(rect, imgBg);
    p.drawImage(rect, imgBoard);
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
    return;    
}

void Canvas::setHoverShape(ShapeBase* shape)
{
    if (!timerDragger) {
        timerDragger = new QTimer(this);
        timerDragger->setInterval(800);
        timerDragger->setSingleShot(true);
        connect(timerDragger, &QTimer::timeout, [this]() {
            if (shapeHover && shapeHover->hoverDraggerIndex != -1) {
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
        if (shape == s || s->state == ShapeState::undo) continue;
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
		win->toolMain->setBtnEnable("undo", true);
	}
}

void Canvas::removeShapeCur()
{
    shapes.removeOne(shapeCur);
    shapeCur->deleteLater();
    shapeCur = nullptr;
}

void Canvas::removeShapeHover()
{
    if (shapeHover == nullptr) return;
    shapeHover->state = ShapeState::undo;
    auto win = (WinBase*)parent();
    imgBoard.fill(Qt::transparent);
    QPainter p(&imgBoard);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    for (auto& s : shapes)
    {
        if (s->state == ShapeState::undo) continue;
        s->paint(&p);
    }
    shapeHover = nullptr;
    win->update();
}

void Canvas::copyColor(const int& key)
{
    if (key == 3) {
        POINT pos;
        GetCursorPos(&pos);
        std::wstring tarStr = QString("%1,%2").arg(pos.x).arg(pos.y).toStdWString();
        Util::setClipboardText(tarStr);
        return;
    }
    Util::copyColor(key);
    qApp->exit(4+key);
}

void Canvas::resize(const QSize& size)
{
	auto win = (WinBase*)parent();
	auto dpr = win->devicePixelRatio();
    imgBg = imgBg.scaled(size*dpr);
    imgBoard = imgBoard.scaled(size * dpr, Qt::KeepAspectRatio);
    imgCanvas = imgCanvas.scaled(size * dpr, Qt::KeepAspectRatio);
}
