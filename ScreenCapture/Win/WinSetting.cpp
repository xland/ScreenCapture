@@
 #include "Lang.h"
 #include "Util.h"
+#include "Win/InputLangHelper.h"
@@
-        winSetting->show();
-        SetForegroundWindow(winSetting->hwnd);
+        winSetting->show();
+        // 不强制把设置窗口置为前台，避免改变系统输入法显示；如果确实需要激活，可改为可选行为
+        // SetForegroundWindow(winSetting->hwnd);
     }
@@
     show();
 }
