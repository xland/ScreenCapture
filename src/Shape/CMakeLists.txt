set(SHAPE
    ShapeBase.cpp
    ShapeBase.h
    Rect.cpp
    Rect.h
    Ellipse.cpp
    Ellipse.h
    Pen.cpp
    Pen.h
    Arrow.cpp
    Arrow.h
)
target_sources(ScreenCapture PRIVATE ${SHAPE})