#include "Lang.h"

static std::map<Lang::Key, std::wstring> lanDic;

Lang::Lang()
{
}

Lang::~Lang()
{
}

void Lang::Init(std::wstring& langType)
{
	if (langType == L"zh_cn") {
		init_zh_cn();
	}
	else if (langType == L"zh_tw") {
		init_zh_tw();
	}
	else if (langType == L"en_us") {
		init_en_us();
	}
	else {
		init_en_us(); //todo
	}
}

std::wstring Lang::Get(Key key)
{
	return lanDic[key];
}

void Lang::init_zh_cn() {
	lanDic.insert({ Key::BtnRect, L"矩形" });
	lanDic.insert({ Key::BtnEllipse, L"圆形" });
	lanDic.insert({ Key::BtnArrow, L"箭头" });
	lanDic.insert({ Key::BtnNumber, L"标号" });
	lanDic.insert({ Key::BtnPen, L"画笔" });
	lanDic.insert({ Key::BtnLine, L"直线" });
	lanDic.insert({ Key::BtnText, L"文本" });
	lanDic.insert({ Key::BtnMosaic, L"马赛克" });
	lanDic.insert({ Key::BtnEraser, L"橡皮擦" });
	lanDic.insert({ Key::BtnUndo, L"上一步(Ctrl+Z)" });
	lanDic.insert({ Key::BtnRedo, L"下一步(Ctrl+Y)" });
	lanDic.insert({ Key::BtnPin, L"钉住截图区(Ctrl+P)" });
	lanDic.insert({ Key::BtnSave, L"保存为文件(Ctrl+S)" });
	lanDic.insert({ Key::BtnCopy, L"保存到剪切板(Ctrl+C)" });
	lanDic.insert({ Key::BtnClose, L"退出(Esc)" });
	lanDic.insert({ Key::BtnTool, L"工具(Ctrl+T)" });

	lanDic.insert({ Key::Left, L"左" });
	lanDic.insert({ Key::Top, L"上" });
	lanDic.insert({ Key::Right, L"右" });
	lanDic.insert({ Key::Bottom, L"下" });
	lanDic.insert({ Key::Width, L"宽" });
	lanDic.insert({ Key::Height, L"高" });
	lanDic.insert({ Key::Position, L"位置" });
}

void Lang::init_zh_tw()
{
	lanDic.insert({ Key::BtnRect, L"矩形" });
	lanDic.insert({ Key::BtnEllipse, L"圓形" });
	lanDic.insert({ Key::BtnArrow, L"箭頭" });
	lanDic.insert({ Key::BtnNumber, L"標號" });
	lanDic.insert({ Key::BtnPen, L"畫筆" });
	lanDic.insert({ Key::BtnLine, L"直綫" });
	lanDic.insert({ Key::BtnText, L"文本" });
	lanDic.insert({ Key::BtnMosaic, L"馬賽克" });
	lanDic.insert({ Key::BtnEraser, L"橡皮擦" });
	lanDic.insert({ Key::BtnUndo, L"上一步(Ctrl+Z)" });
	lanDic.insert({ Key::BtnRedo, L"下一步(Ctrl+Y)" });
	lanDic.insert({ Key::BtnPin, L"釘住截圖區(Ctrl+P)" });
	lanDic.insert({ Key::BtnSave, L"保存為文件(Ctrl+S)" });
	lanDic.insert({ Key::BtnCopy, L"保存到剪贴板(Ctrl+C)" });
	lanDic.insert({ Key::BtnClose, L"退出(Esc)" });
	lanDic.insert({ Key::BtnTool, L"工具(Ctrl+T)" });

	lanDic.insert({ Key::Left, L"左" });
	lanDic.insert({ Key::Top, L"上" });
	lanDic.insert({ Key::Right, L"右" });
	lanDic.insert({ Key::Bottom, L"下" });
	lanDic.insert({ Key::Width, L"寬" });
	lanDic.insert({ Key::Height, L"高" });
	lanDic.insert({ Key::Position, L"位置" });
}

void Lang::init_en_us()
{
	lanDic.insert({ Key::BtnRect, L"Rectangle" });
	lanDic.insert({ Key::BtnEllipse, L"Ellipse" });
	lanDic.insert({ Key::BtnArrow, L"Arrow" });
	lanDic.insert({ Key::BtnNumber, L"Number" });
	lanDic.insert({ Key::BtnPen, L"Pen" });
	lanDic.insert({ Key::BtnLine, L"Line" });
	lanDic.insert({ Key::BtnText, L"Text" });
	lanDic.insert({ Key::BtnMosaic, L"Mosaic" });
	lanDic.insert({ Key::BtnEraser, L"Eraser" });
	lanDic.insert({ Key::BtnUndo, L"Undo(Ctrl+Z)" });
	lanDic.insert({ Key::BtnRedo, L"Redo(Ctrl+Y)" });
	lanDic.insert({ Key::BtnPin, L"Pin cut Area(Ctrl+P)" });
	lanDic.insert({ Key::BtnSave, L"Save to file(Ctrl+S)" });
	lanDic.insert({ Key::BtnCopy, L"Save to clipboard(Ctrl+C)" });
	lanDic.insert({ Key::BtnClose, L"Exit(Esc)" });
	lanDic.insert({ Key::BtnTool, L"Tool(Ctrl+T)" });

	lanDic.insert({ Key::Left, L"Left" });
	lanDic.insert({ Key::Top, L"Top" });
	lanDic.insert({ Key::Right, L"Right" });
	lanDic.insert({ Key::Bottom, L"Bottom" });
	lanDic.insert({ Key::Width, L"Width" });
	lanDic.insert({ Key::Height, L"Height" });
	lanDic.insert({ Key::Position, L"Position" });
}

void Lang::init_ja()
{
}


void Lang::init_en_gb()
{
}
