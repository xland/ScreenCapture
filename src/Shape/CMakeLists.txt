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
    ShapePen.cpp
    ShapePen.h
    ShapeLine.cpp
    ShapeLine.h
)
target_sources(ScreenCapture PRIVATE ${SHAPE})