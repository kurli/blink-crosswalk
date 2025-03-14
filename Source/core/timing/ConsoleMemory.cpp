// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/timing/ConsoleMemory.h"

#include "core/frame/Console.h"
#include "core/timing/MemoryInfo.h"

namespace blink {

DEFINE_EMPTY_DESTRUCTOR_WILL_BE_REMOVED(ConsoleMemory);

DEFINE_TRACE(ConsoleMemory)
{
    visitor->trace(m_memory);
    WillBeHeapSupplement<Console>::trace(visitor);
}

// static
ConsoleMemory& ConsoleMemory::from(Console& console)
{
    ConsoleMemory* supplement = static_cast<ConsoleMemory*>(WillBeHeapSupplement<Console>::from(console, supplementName()));
    if (!supplement) {
        supplement = new ConsoleMemory();
        provideTo(console, supplementName(), adoptPtrWillBeNoop(supplement));
    }
    return *supplement;
}

// static
MemoryInfo* ConsoleMemory::memory(Console& console)
{
    return ConsoleMemory::from(console).memory();
}

MemoryInfo* ConsoleMemory::memory()
{
    if (!m_memory)
        m_memory = MemoryInfo::create();

    return m_memory.get();
}

} // namespace blink
