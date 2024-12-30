- 按住Shift键，画马赛克线条时，拖拽会扫过一大片区域
- 主工具栏要保持在屏幕内，动态调整位置
- maskRect的info信息在多屏情况下，仅判断y>0，可能会显示不全
- Ctrl+Z,Ctrl+Y
- 有的时候del无法删除元素
- 鼠标移出Pin窗口，要触发release事件，不然一直处于拖动改变shape状态
- 拷贝颜色信息位置信息
- Pin窗口不能写字
- full会画到mask上面
- 改变mask到顶部  toolbar不会出现在底部



- 通过命令行截图
- 命令行截图应该支持直接退出进程
- 是否可以在WinBase的Pint方法里直接释放img

- Pin窗口右键菜单应该有是否显示PixInfo的选项
- 自由线条绘制的不是很平滑
- 书写文本时，允许用户选择字体
- 按Shif键时，水平，45度，垂直磁铁效果
- 搞一套日志系统
- 鼠标移动时不断调用QGuiApplication::setOverrideCursor值不值得优化
- 上一步、下一步功能单个Shape也应该存储一个history list
- Pin窗口 绘图会画到阴影上
- Pin窗口，拖拽绘图，超出窗口区域再释放鼠标，会导致绘图不会终结
- 首次绘制不应该允许用户绘制橡皮擦
- 改变子工具栏内的选项，当前shape也跟着改变
- tooltip 延迟显示，防抖，鼠标移开时不显示
- pin win右键菜单可以用Qt技术实现? (不存在大小的问题)(要点两次才会出现)
- pin窗口也可以显示PixInfo
- Pin窗口获取像素值
- Pin窗口PixInfo显示在右下时，定位有点问题
- hsv color
- 