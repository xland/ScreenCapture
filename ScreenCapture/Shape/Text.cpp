#include "Text.h"


namespace Shape {
    void Text::Draw(BLContext* paintCtx, const double& x1, const double& y1, const double& x2, const double& y2)
    {       
        paintCtx->setStrokeStyle(color);
        paintCtx->setStrokeWidth(3);
        if (startX == -1) {
            startX = x1 - 18;
            startY = y1 - 38;
        }
        if (text.empty()) {
            endX = x1 + 18;
            endY = y1 + 38;
        }
        else
        {

        }
        paintCtx->strokeBox(startX, startY, endX, endY);
        if (showInputCursor) {
            paintCtx->strokeLine(startX + 18, startY + 18, startX + 18, endY - 18);
        }
        showInputCursor = !showInputCursor;
    }
}