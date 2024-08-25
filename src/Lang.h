#include <map>
#include <string>

class Lang
{
public:
	enum class Key
	{
		BtnRect = 0,
		BtnEllipse,
		BtnArrow,
		BtnNumber,
		BtnPen,
		BtnLine,
		BtnText,
		BtnMosaic,
		BtnEraser,
		BtnUndo,
		BtnRedo,
		BtnPin,
		BtnSave,
		BtnCopy,
		BtnClose,
		BtnTool,

		Left,
		Top,
		Right,
		Bottom,
		Width,
		Height,
		Position
	};
	~Lang();
	static void Init();
	static std::wstring Get(Key key);
private:
	Lang();
	static void init_zh_cn();
	static void init_zh_tw();
	static void init_en_gb();
	static void init_en_us();
	static void init_ja();
};