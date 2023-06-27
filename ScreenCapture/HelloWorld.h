/*
* Copyright 2017 Google Inc.
*
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE file.
*/

#ifndef HelloWorld_DEFINED
#define HelloWorld_DEFINED

#include "include/core/SkScalar.h"
#include "include/core/SkTypes.h"
#include "sk_app/Window.h"
#include "tools/skui/ModifierKey.h"

class SkSurface;

class HelloWorld : sk_app::Window::Layer {
public:
    HelloWorld(HINSTANCE hinstance);
    ~HelloWorld() override;

    void onIdle();

    void onBackendCreated() override;
    void onPaint(SkSurface*) override;
    bool onChar(SkUnichar c, skui::ModifierKey modifiers) override;

private:
    void updateTitle();

    sk_app::Window* fWindow;
    sk_app::Window::BackendType fBackendType;

    SkScalar fRotationAngle;
};

#endif
