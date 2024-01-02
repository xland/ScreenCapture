 - 
 - track mouse out window IsMouseDragging IsMouseDown to false
 - 橡皮擦画在front上和画在back上表现不一样，鼠标up事件仔细观察一下
 - 用del 删除元素
 - 辅助线
 - 截图框
   - 截图Info
   - 蓝色边框
   - 键盘控制截图区域位置
   - mask上加8个圆，用于开始绘图之后，修改截图区域
   - 高亮系统中的窗口区域
- GPU SUPPORT
- 工具栏取消选中之后再拖动元素，是有问题的


- undo redo的时候,在SkPictureRecorder对象中重放绘制操作会更快
- SK_ENABLE_SKSL 是 Skia 中的一个宏，用于启用或禁用 Skia Shader Language（SKSL）编译器。SKSL 是 Skia 的着色器语言，用于编写图形渲染时使用的着色器代码。
  - 看SkiaGUI有使用这玩意儿的示例



- 写字时，鼠标移动到其他元素上时，必不能显示dragger，因为dragger会刷新front canvas，就会把字刷掉
- 写字时，鼠标左键点击文字Rect以外的区域，应无任何反应
- 右键点击文字Rect以外的区域或点击工具栏结束写字
- 字必不能写到back canvas上，因为删字时，要刷新整个画布
- 鼠标移入历史记录的字的Rect点击，或拖拽都会重新编辑字