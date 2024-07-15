1. 上一步之后就不能修改元素标号了
2. 标号元素上一步之后，再创建新的，标号数字有点问题
3. 有的时候不会出现工具栏



- 直接钉住剪切板内的图片
  - 读取剪切板内的图片




- 应该把钢笔和直线合并成一个（工具栏）
- Doc改为doc
- 空白的文字框要拖拽，怎么办
- 保存成bmp不好吗
- 每次渲染时临时创建bixmap内存也不迟
- 参考modules\skplaintexteditor\app\editor_application.cpp
  
 

- 截图: 保存按钮 希望可以定义默认文件夹, 不用每次都选文件夹
- 当截图区宽度等于屏幕宽度，或高度等于屏幕高度时，没办法再调整截图区域的宽高

- 感觉有点卡，尤其在虚拟机里，恼火
- ShapeEllipse ShapeRect判断鼠标坐标是否移动到元素内，这里可以优化，提前把Path缓存起来
- Cursor的枚举类值得优化，提前Load：HCURSOR hCursor = LoadCursor(nullptr, IDC_ARROW); // 或加载自定义光标资源
- draggerSize设置成静态变量
- 英文文字的测量是由问题的
- 多个屏幕测试，要多测试几轮
- Eraser和Mosaic 是否应继承自Pen


- 别的程序调用，截图后好像获取不到文件保存路径
- Recorder有必要早早的就初始化吗？Get的时候判断，没有再初始化不就完了
- 用del 删除元素
- 辅助线
- GPU SUPPORT


- SK_ENABLE_SKSL 是 Skia 中的一个宏，用于启用或禁用 Skia Shader Language（SKSL）编译器。SKSL 是 Skia 的着色器语言，用于编写图形渲染时使用的着色器代码。
  - 看SkiaGUI有使用这玩意儿的示例
 - track mouse out window IsMouseDragging IsMouseDown to false



- 整个应用只搞一个Surface，一个Canvas
- 屏幕拍照完成后，就把屏幕照片画在这个Surface上
- 然后saveLayer①，
- 接着在这个layer上绘制mask,toolbar之类得东西
- 动态绘制这些东西时，就执行clear操作，不会影响最底层得屏幕照片
- 完成之后restore，（完蛋，污染了surface了）
- 如果restore了之后，还要改变mask或toolbar，那就重新画一遍屏幕底部得照片
- 然后再saveLayer①，再改变mask或toolbar，
- 动态改变就执行clear操作，同样不会影响最底层得屏幕照片
- 完成之后restore

- 在①完成之后，你可能要绘制历史元素
- 此时没必要再saveLayer
- 只要直接遍历历史元素，直接并把非IsDel的元素都绘制到①创建的layer上即可
- 画完了之后restore
- 当用户点了上一步或者下一步，那么整个surface都要重绘一遍

- 当用户要拖拽绘制一个新元素时
- 不断的saveLayer,clear,draw,restore
- 一旦restore，就会污染surface了
- 然而如果不restore，用户又看不到拖拽绘制的过程



toolbar可以搞一个单独的surface

- 用SaveLayer代替SurfaceFront
  - 当开始绘图时，不断的saveLayer,clear,draw,restore
  - 实际上始终是画在SurfaceBack上
  - 