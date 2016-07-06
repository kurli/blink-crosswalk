// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "core/dom/ExceptionCode.h"
#include "modules/three/Three.h"
#include "core/three/three/three.h"
#include "modules/three/CWGraphicsContext.h"
// #include "core/three/three/three.hpp"


#define FUNC_NAME(a, b) a ## b
#define ATTRIBUTE_GETTER_CUSTOM(_class_) \
    void V8Three:: FUNC_NAME(_class_, AttributeGetterCustom) (v8::FunctionCallbackInfo<v8::Value> const& info) { \
        v8::Isolate* isolate = info.GetIsolate(); \
        v8::Handle<v8::FunctionTemplate> functionTemplate = V8##_class_::domTemplate(isolate); \
        v8::Handle<v8::Function> v8Function = functionTemplate->GetFunction(); \
        v8SetReturnValue(info, v8Function); \
    }

using namespace three;

namespace blink {

Three::Three()
{
}

Three::~Three()
{
}

DEFINE_TRACE(Three) {
}

void Three::forceGC() {
    v8::Isolate::GetCurrent()->LowMemoryNotification();
}

void Three::init(WebGLRenderingContext* context, int width, int height) {
	blink::CWGraphicsContext::mContext = context;
    three::ExampleSession::init(width, height);
}
void Three::run() {
    three::ExampleSession::run();
}

void Three::step(double dt) {
    three::ExampleSession::step(dt);
}

}