/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include "platform/EventTracer.h"
#include "platform/TestingPlatformSupport.h"
#include "platform/heap/Heap.h"
#include "wtf/CryptographicallyRandomNumber.h"
#include "wtf/MainThread.h"
#include "wtf/Partitions.h"
#include "wtf/WTF.h"
#include <base/test/test_suite.h>
#include <string.h>

static double CurrentTime()
{
    return 0.0;
}

static void AlwaysZeroNumberSource(unsigned char* buf, size_t len)
{
    memset(buf, '\0', len);
}

int main(int argc, char** argv)
{
    WTF::setRandomSource(AlwaysZeroNumberSource);
    WTF::initialize(CurrentTime, 0, 0);
    WTF::initializeMainThread(0);

    blink::TestingPlatformSupport::Config platformConfig;
    blink::TestingPlatformSupport platform(platformConfig);

    blink::Heap::init();
    blink::ThreadState::attachMainThread();
    blink::EventTracer::initialize();
    int result = base::RunUnitTestsUsingBaseTestSuite(argc, argv);
    blink::ThreadState::detachMainThread();
    blink::Heap::shutdown();

    WTF::shutdown();
    return result;
}
