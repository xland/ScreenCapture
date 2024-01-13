- Bug fix: Cann't write cross-screen shot to the clipboard.
- Bug fix: Remove single instance control.
- Bug fix: Rect mosaic area reach screen edge crash.
- Bug fix: Sub tool bar obscure the cut area when main tool bar is above the cut area.
- Bug fix: Quick click toolbar button trigger double-click event.
- Bug fix: Draw by pen with wrong stroke width.
- Bug fix: Pin window can not exit by context menu.
- Bug fix: Pin window position error when user have more than one screen.
- Bug fix: Reset main tool when pin window.
- Some code optimization.

- 绘图过程中屏幕会黑一下的问题又来了，应该是重绘并发导致的
- 绘制马赛克后，马赛克以前的历史元素变更位置，马赛克遮不住他们
- 钉住窗口再画Rect会多一个上一次画的东西

- 多个屏幕，马赛克，auto currentColor = pixmap->getColor4f(x1, y1);  x1和y1有可能是负值，这样取到的color有问题
- 多个屏幕测试，要多测试几轮




- Eraser和Mosaic 是否应继承自Pen




- 橡皮擦画在front上和画在back上表现不一样，鼠标up事件仔细观察一下
- ShapeDragger Recorder Timer这类东西有必要早早的就初始化吗？Get的时候判断，没有再初始化不就完了
- 工具栏取消选中之后再拖动元素，是有问题的
- timer有问题，进程结束，还在调用异步方法
- CutBox完成之后，点工具栏会改变CutBox，偶发



 - 用del 删除元素
 - 辅助线
- GPU SUPPORT


- SK_ENABLE_SKSL 是 Skia 中的一个宏，用于启用或禁用 Skia Shader Language（SKSL）编译器。SKSL 是 Skia 的着色器语言，用于编写图形渲染时使用的着色器代码。
  - 看SkiaGUI有使用这玩意儿的示例
 - track mouse out window IsMouseDragging IsMouseDown to false


- 写字时，鼠标移动到其他元素上时，必不能显示dragger，因为dragger会刷新front canvas，就会把字刷掉
- 写字时，鼠标左键点击文字Rect以外的区域，应无任何反应
- 右键点击文字Rect以外的区域或点击工具栏结束写字
- 字必不能写到back canvas上，因为删字时，要刷新整个画布
- 鼠标移入历史记录的字的Rect点击，或拖拽都会重新编辑字