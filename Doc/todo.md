 - 橡皮擦画在front上和画在back上表现不一样，鼠标up事件仔细观察一下
 - 用del 删除元素
 - 辅助线
 - 截图框
   - 键盘控制截图区域位置
   - 高亮系统中的窗口区域
 - timer recorder这类东西有必要早早的就初始化吗？Get的时候判断，没有再初始化不就完了
 - WindowMain::get有点问题，应该时WindowBase::get，这样对于钉住着的窗口更友好
- GPU SUPPORT
- 工具栏取消选中之后再拖动元素，是有问题的
- 在制品元素未Drag时要删除
- Text输入有问题，看看就知道了
- timer有问题，进程结束，还在调用异步方法
- CutBox完成之后，点工具栏会改变CutBox，偶发
- 点了输入文本按钮之后，鼠标光标应改为input






- SK_ENABLE_SKSL 是 Skia 中的一个宏，用于启用或禁用 Skia Shader Language（SKSL）编译器。SKSL 是 Skia 的着色器语言，用于编写图形渲染时使用的着色器代码。
  - 看SkiaGUI有使用这玩意儿的示例
 - track mouse out window IsMouseDragging IsMouseDown to false


- 写字时，鼠标移动到其他元素上时，必不能显示dragger，因为dragger会刷新front canvas，就会把字刷掉
- 写字时，鼠标左键点击文字Rect以外的区域，应无任何反应
- 右键点击文字Rect以外的区域或点击工具栏结束写字
- 字必不能写到back canvas上，因为删字时，要刷新整个画布
- 鼠标移入历史记录的字的Rect点击，或拖拽都会重新编辑字