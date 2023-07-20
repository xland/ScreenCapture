#include "Util.h"

void SetBoxByPos(BLBox& box, const double& x1, const double& y1, const double& x2, const double& y2)
{
    if (x1 < x2) {
        box.x0 = x1;
        box.x1 = x2;
    }
    else
    {
        box.x0 = x2;
        box.x1 = x1;
    }
    if (y1 < y2) {
        box.y0 = y1;
        box.y1 = y2;
    }
    else
    {
        box.y0 = y2;
        box.y1 = y1;
    }
}
