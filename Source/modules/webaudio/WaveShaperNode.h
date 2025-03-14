/*
 * Copyright (C) 2011, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WaveShaperNode_h
#define WaveShaperNode_h

#include "core/dom/DOMTypedArray.h"
#include "modules/webaudio/AudioBasicProcessorNode.h"
#include "modules/webaudio/BiquadProcessor.h"
#include "modules/webaudio/WaveShaperProcessor.h"

namespace blink {

class ExceptionState;

class WaveShaperNode final : public AudioNode {
    DEFINE_WRAPPERTYPEINFO();
public:
    static WaveShaperNode* create(AudioContext* context)
    {
        return new WaveShaperNode(context);
    }

    // setCurve() is called on the main thread.
    void setCurve(DOMFloat32Array*, ExceptionState&);
    DOMFloat32Array* curve();

    void setOversample(const String&);
    String oversample() const;

private:
    explicit WaveShaperNode(AudioContext*);

    WaveShaperProcessor* waveShaperProcessor() const;
};

} // namespace blink

#endif // WaveShaperNode_h
