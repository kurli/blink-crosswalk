/*
 * Copyright (C) 2010 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef SchemeRegistry_h
#define SchemeRegistry_h

#include "platform/PlatformExport.h"
#include "wtf/HashMap.h"
#include "wtf/HashSet.h"
#include "wtf/text/StringHash.h"
#include "wtf/text/WTFString.h"

namespace blink {

using URLSchemesSet = HashSet<String, CaseFoldingHash>;

template <typename T>
using URLSchemesMap = HashMap<String, T, CaseFoldingHash>;

class PLATFORM_EXPORT SchemeRegistry {
public:
    static void registerURLSchemeAsLocal(const String&);
    static bool shouldTreatURLSchemeAsLocal(const String&);

    static void registerURLSchemeAsRestrictingMixedContent(const String&);
    static bool shouldTreatURLSchemeAsRestrictingMixedContent(const String&);

    // Secure schemes do not trigger mixed content warnings. For example,
    // https and data are secure schemes because they cannot be corrupted by
    // active network attackers.
    static void registerURLSchemeAsSecure(const String&);
    static bool shouldTreatURLSchemeAsSecure(const String&);

    static void registerURLSchemeAsNoAccess(const String&);
    static bool shouldTreatURLSchemeAsNoAccess(const String&);

    // Display-isolated schemes can only be displayed (in the sense of
    // SecurityOrigin::canDisplay) by documents from the same scheme.
    static void registerURLSchemeAsDisplayIsolated(const String&);
    static bool shouldTreatURLSchemeAsDisplayIsolated(const String&);

    static void registerURLSchemeAsEmptyDocument(const String&);
    static bool shouldLoadURLSchemeAsEmptyDocument(const String&);

    static void setDomainRelaxationForbiddenForURLScheme(bool forbidden, const String&);
    static bool isDomainRelaxationForbiddenForURLScheme(const String&);

    // Such schemes should delegate to SecurityOrigin::canRequest for any URL
    // passed to SecurityOrigin::canDisplay.
    static bool canDisplayOnlyIfCanRequest(const String& scheme);

    // Schemes against which javascript: URLs should not be allowed to run (stop
    // bookmarklets from running on sensitive pages).
    static void registerURLSchemeAsNotAllowingJavascriptURLs(const String& scheme);
    static bool shouldTreatURLSchemeAsNotAllowingJavascriptURLs(const String& scheme);

    // Allow non-HTTP schemes to be registered to allow CORS requests.
    static void registerURLSchemeAsCORSEnabled(const String& scheme);
    static bool shouldTreatURLSchemeAsCORSEnabled(const String& scheme);

    // Serialize the registered schemes in a comma-separated list.
    static String listOfCORSEnabledURLSchemes();

    // "Legacy" schemes (e.g. 'ftp:', 'gopher:') which we might want to treat differently from "webby" schemes.
    static bool shouldTreatURLSchemeAsLegacy(const String& scheme);

    // Allow resources from some schemes to load on a page, regardless of its
    // Content Security Policy.
    // This enum should be kept in sync with public/web/WebSecurityPolicy.h.
    // Enforced in AssertMatchingEnums.cpp.
    enum PolicyAreas : uint32_t {
        PolicyAreaNone = 0,
        PolicyAreaImage = 1 << 0,
        PolicyAreaStyle = 1 << 1,
        // Add more policy areas as needed by clients.
        PolicyAreaAll = ~static_cast<uint32_t>(0),
    };
    static void registerURLSchemeAsBypassingContentSecurityPolicy(const String& scheme, PolicyAreas = PolicyAreaAll);
    static void removeURLSchemeRegisteredAsBypassingContentSecurityPolicy(const String& scheme);
    static bool schemeShouldBypassContentSecurityPolicy(const String& scheme, PolicyAreas = PolicyAreaAll);

private:
    static const URLSchemesSet& localSchemes();
};

} // namespace blink

#endif // SchemeRegistry_h
