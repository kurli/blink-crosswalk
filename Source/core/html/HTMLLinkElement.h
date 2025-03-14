/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003, 2008, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef HTMLLinkElement_h
#define HTMLLinkElement_h

#include "core/css/CSSStyleSheet.h"
#include "core/dom/DOMSettableTokenList.h"
#include "core/dom/IconURL.h"
#include "core/fetch/ResourceOwner.h"
#include "core/fetch/StyleSheetResource.h"
#include "core/fetch/StyleSheetResourceClient.h"
#include "core/html/HTMLElement.h"
#include "core/html/LinkRelAttribute.h"
#include "core/html/LinkResource.h"
#include "core/loader/LinkLoader.h"
#include "core/loader/LinkLoaderClient.h"

namespace blink {

class HTMLLinkElement;
class KURL;
class LinkImport;

template<typename T> class EventSender;
typedef EventSender<HTMLLinkElement> LinkEventSender;

//
// LinkStyle handles dynaically change-able link resources, which is
// typically @rel="stylesheet".
//
// It could be @rel="shortcut icon" or soething else though. Each of
// types might better be handled by a separate class, but dynamically
// changing @rel makes it harder to move such a design so we are
// sticking current way so far.
//
class LinkStyle final : public LinkResource, ResourceOwner<StyleSheetResource> {
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(LinkStyle);
public:
    static PassOwnPtrWillBeRawPtr<LinkStyle> create(HTMLLinkElement* owner);

    explicit LinkStyle(HTMLLinkElement* owner);
    virtual ~LinkStyle();

    virtual Type type() const override { return Style; }
    virtual void process() override;
    virtual void ownerRemoved() override;
    virtual bool hasLoaded() const override { return m_loadedSheet; }
    DECLARE_VIRTUAL_TRACE();

    void startLoadingDynamicSheet();
    void notifyLoadedSheetAndAllCriticalSubresources(Node::LoadedSheetErrorStatus);
    bool sheetLoaded();

    void setDisabledState(bool);
    void setSheetTitle(const String&);

    bool styleSheetIsLoading() const;
    bool hasSheet() const { return m_sheet; }
    bool isDisabled() const { return m_disabledState == Disabled; }
    bool isEnabledViaScript() const { return m_disabledState == EnabledViaScript; }
    bool isUnset() const { return m_disabledState == Unset; }

    CSSStyleSheet* sheet() const { return m_sheet.get(); }

private:
    // From StyleSheetResourceClient
    virtual void setCSSStyleSheet(const String& href, const KURL& baseURL, const String& charset, const CSSStyleSheetResource*) override;

    enum DisabledState {
        Unset,
        EnabledViaScript,
        Disabled
    };

    enum PendingSheetType {
        None,
        NonBlocking,
        Blocking
    };

    void clearSheet();
    void addPendingSheet(PendingSheetType);
    void removePendingSheet();
    Document& document();

    void setCrossOriginStylesheetStatus(CSSStyleSheet*);
    void setFetchFollowingCORS()
    {
        ASSERT(!m_fetchFollowingCORS);
        m_fetchFollowingCORS = true;
    }

    RefPtrWillBeMember<CSSStyleSheet> m_sheet;
    DisabledState m_disabledState;
    PendingSheetType m_pendingSheetType;
    bool m_loading;
    bool m_firedLoad;
    bool m_loadedSheet;
    bool m_fetchFollowingCORS;
};


class HTMLLinkElement final : public HTMLElement, public LinkLoaderClient {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<HTMLLinkElement> create(Document&, bool createdByParser);
    virtual ~HTMLLinkElement();

    KURL href() const;
    const AtomicString& rel() const;
    String media() const { return m_media; }
    String typeValue() const { return m_type; }
    const LinkRelAttribute& relAttribute() const { return m_relAttribute; }

    const AtomicString& type() const;

    IconType iconType() const;

    // the icon sizes as parsed from the HTML attribute
    const Vector<IntSize>& iconSizes() const;

    bool async() const;

    CSSStyleSheet* sheet() const { return linkStyle() ? linkStyle()->sheet() : 0; }
    Document* import() const;

    bool styleSheetIsLoading() const;

    bool isImport() const { return linkImport(); }
    bool isDisabled() const { return linkStyle() && linkStyle()->isDisabled(); }
    bool isEnabledViaScript() const { return linkStyle() && linkStyle()->isEnabledViaScript(); }
    void setEnabledIfExitTransitionStyle(bool);

    DOMSettableTokenList* sizes() const;

    void dispatchPendingEvent(LinkEventSender*);
    void scheduleEvent();
    void dispatchEventImmediately();
    static void dispatchPendingLoadEvents();

    // From LinkLoaderClient
    virtual bool shouldLoadLink() override;

    // For LinkStyle
    bool loadLink(const String& type, const KURL&);
    bool isAlternate() const { return linkStyle()->isUnset() && m_relAttribute.isAlternate(); }
    bool shouldProcessStyle() { return linkResourceToProcess() && linkStyle(); }
    bool isCreatedByParser() const { return m_createdByParser; }

    // Parse the icon size attribute into |iconSizes|, make this method public
    // visible for testing purpose.
    static void parseSizesAttribute(const AtomicString& value, Vector<IntSize>& iconSizes);

    DECLARE_VIRTUAL_TRACE();

private:
    virtual void attributeWillChange(const QualifiedName&, const AtomicString& oldValue, const AtomicString& newValue) override;
    virtual void parseAttribute(const QualifiedName&, const AtomicString&) override;

    LinkStyle* linkStyle() const;
    LinkImport* linkImport() const;
    LinkResource* linkResourceToProcess();

    void process();
    static void processCallback(Node*);

    // From Node and subclassses
    virtual InsertionNotificationRequest insertedInto(ContainerNode*) override;
    virtual void removedFrom(ContainerNode*) override;
    virtual bool isURLAttribute(const Attribute&) const override;
    virtual bool hasLegalLinkAttribute(const QualifiedName&) const override;
    virtual const QualifiedName& subResourceAttributeName() const override;
    virtual bool sheetLoaded() override;
    virtual void notifyLoadedSheetAndAllCriticalSubresources(LoadedSheetErrorStatus) override;
    virtual void startLoadingDynamicSheet() override;
    virtual void finishParsingChildren() override;

    // From LinkLoaderClient
    virtual void linkLoaded() override;
    virtual void linkLoadingErrored() override;
    virtual void didStartLinkPrerender() override;
    virtual void didStopLinkPrerender() override;
    virtual void didSendLoadForLinkPrerender() override;
    virtual void didSendDOMContentLoadedForLinkPrerender() override;

private:
    HTMLLinkElement(Document&, bool createdByParser);

    OwnPtrWillBeMember<LinkResource> m_link;
    LinkLoader m_linkLoader;

    String m_type;
    String m_media;
    RefPtrWillBeMember<DOMSettableTokenList> m_sizes;
    Vector<IntSize> m_iconSizes;
    LinkRelAttribute m_relAttribute;

    bool m_createdByParser;
    bool m_isInShadowTree;
};

} // namespace blink

#endif // HTMLLinkElement_h
