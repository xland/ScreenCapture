@@
 #include "Tool/ToolSub.h"
 #include "History.h"
 #include "Shape/ShapeBase.h"
@@
 #include "Shape/ShapeEraser.h"
+#include "Win/InputLangHelper.h"
@@
     if (foregroundBeforeDialog && foregroundBeforeDialog != hwnd && IsWindow(foregroundBeforeDialog) && IsWindowVisible(foregroundBeforeDialog)) {
-        SetForegroundWindow(foregroundBeforeDialog);
+        // 保存当前输入法布局，切换前台窗口后再恢复，避免系统托盘输入法图标被改变
+        InputLangHelper::SaveKeyboardLayoutForWindow(GetForegroundWindow());
+        SetForegroundWindow(foregroundBeforeDialog);
+        InputLangHelper::RestoreSavedKeyboardLayout();
     }
 }
