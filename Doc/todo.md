- Hover over the area of the history element and wait for a moment before entering modify mode.
- Use Windows `SetTimer`` and `KillTimer`` instead of std::thread
- Remove `ShapeDragger` that is no longer in use.


- 修改马赛克之后，马赛克变成最上册的元素了
- 
- Visual Studio分析和代码清理
- ShapeEllipse ShapeRect判断鼠标坐标是否移动到元素内，这里可以优化，提前把Path缓存起来
- Cursor的枚举类值得优化，提前Load：HCURSOR hCursor = LoadCursor(nullptr, IDC_ARROW); // 或加载自定义光标资源
- draggerSize设置成静态变量
- Eraser Rect 拖动会擦掉所有内容，mouseup之后才会显示出来
- 英文文字的测量是由问题的
- 点工具条的时候会导致Crash？？？应该已经修复了
- 按住Shift画正圆，按住Shift画正方
- 绘制马赛克后，马赛克以前的历史元素变更位置，马赛克遮不住他们
- 钉住窗口再画Rect会多一个上一次画的东西
- 多个屏幕测试，要多测试几轮
- Eraser和Mosaic 是否应继承自Pen


- 橡皮擦画在front上和画在back上表现不一样，鼠标up事件仔细观察一下
- Recorder有必要早早的就初始化吗？Get的时候判断，没有再初始化不就完了
- 工具栏取消选中之后再拖动元素，是有问题的
- 用del 删除元素
- 辅助线
- GPU SUPPORT


- SK_ENABLE_SKSL 是 Skia 中的一个宏，用于启用或禁用 Skia Shader Language（SKSL）编译器。SKSL 是 Skia 的着色器语言，用于编写图形渲染时使用的着色器代码。
  - 看SkiaGUI有使用这玩意儿的示例
 - track mouse out window IsMouseDragging IsMouseDown to false