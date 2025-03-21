// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/permissions/NavigatorPermissions.h"

#include "core/frame/Navigator.h"
#include "modules/permissions/Permissions.h"

namespace blink {

NavigatorPermissions::NavigatorPermissions()
{
}

NavigatorPermissions::~NavigatorPermissions()
{
}

// static
const char* NavigatorPermissions::supplementName()
{
    return "NavigatorPermissions";
}

// static
NavigatorPermissions& NavigatorPermissions::from(Navigator& navigator)
{
    NavigatorPermissions* supplement = static_cast<NavigatorPermissions*>(WillBeHeapSupplement<Navigator>::from(navigator, supplementName()));
    if (!supplement) {
        supplement = new NavigatorPermissions();
        provideTo(navigator, supplementName(), adoptPtrWillBeNoop(supplement));
    }
    return *supplement;
}

// static
Permissions* NavigatorPermissions::permissions(Navigator& navigator)
{
    NavigatorPermissions& self = NavigatorPermissions::from(navigator);
    if (!self.m_permissions)
        self.m_permissions = new Permissions();
    return self.m_permissions.get();
}

DEFINE_TRACE(NavigatorPermissions)
{
    visitor->trace(m_permissions);
    WillBeHeapSupplement<Navigator>::trace(visitor);
}

} // namespace blink
