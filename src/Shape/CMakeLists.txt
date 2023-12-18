set(SHAPE
    ShapeBase.cpp
    ShapeBase.h
    ShapeRect.cpp
    ShapeRect.h
    ShapeEllipse.cpp
    ShapeEllipse.h
)
target_sources(ScreenCapture PRIVATE ${SHAPE})