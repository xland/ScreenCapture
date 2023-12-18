set(SHAPE
    ShapeBase.cpp
    ShapeBase.h
    ShapeDragger.cpp
    ShapeDragger.h
    Rect.cpp
    Rect.h
    Pen.cpp
    Pen.h
    Arrow.cpp
    Arrow.h
)
target_sources(ScreenCapture PRIVATE ${SHAPE})