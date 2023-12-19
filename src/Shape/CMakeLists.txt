set(SHAPE
    ShapeBase.cpp
    ShapeBase.h
    ShapeRect.cpp
    ShapeRect.h
    ShapeEllipse.cpp
    ShapeEllipse.h
    ShapeArrow.cpp
    ShapeArrow.h
    ShapeNumber.cpp
    ShapeNumber.h
)
target_sources(ScreenCapture PRIVATE ${SHAPE})