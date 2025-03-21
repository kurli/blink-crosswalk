/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 *           (C) 2006 Alexey Proskuryakov (ap@webkit.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2011, 2012 Apple Inc. All rights reserved.
 * Copyright (C) 2008, 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 * Copyright (C) 2008, 2009, 2011, 2012 Google Inc. All rights reserved.
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) Research In Motion Limited 2010-2011. All rights reserved.
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
 */

#include "config.h"
#include "core/dom/StyleEngine.h"

#include "core/HTMLNames.h"
#include "core/css/CSSFontSelector.h"
#include "core/css/CSSStyleSheet.h"
#include "core/css/FontFaceCache.h"
#include "core/css/StyleSheetContents.h"
#include "core/css/resolver/ScopedStyleResolver.h"
#include "core/dom/DocumentStyleSheetCollector.h"
#include "core/dom/Element.h"
#include "core/dom/ProcessingInstruction.h"
#include "core/dom/ShadowTreeStyleSheetCollection.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLIFrameElement.h"
#include "core/html/HTMLLinkElement.h"
#include "core/html/imports/HTMLImportsController.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/page/InjectedStyleSheets.h"
#include "core/page/Page.h"
#include "core/svg/SVGStyleElement.h"
#include "platform/URLPatternMatcher.h"

namespace blink {

using namespace HTMLNames;

StyleEngine::StyleEngine(Document& document)
    : m_document(&document)
    , m_isMaster(!document.importsController() || document.importsController()->master() == &document)
    , m_pendingStylesheets(0)
    , m_injectedStyleSheetCacheValid(false)
    , m_documentStyleSheetCollection(DocumentStyleSheetCollection::create(document))
    , m_documentScopeDirty(true)
    , m_usesSiblingRules(false)
    , m_usesFirstLineRules(false)
    , m_usesWindowInactiveSelector(false)
    , m_usesFirstLetterRules(false)
    , m_usesRemUnits(false)
    , m_maxDirectAdjacentSelectors(0)
    , m_ignorePendingStylesheets(false)
    , m_didCalculateResolver(false)
    // We don't need to create CSSFontSelector for imported document or
    // HTMLTemplateElement's document, because those documents have no frame.
    , m_fontSelector(document.frame() ? CSSFontSelector::create(&document) : nullptr)
{
    if (m_fontSelector)
        m_fontSelector->registerForInvalidationCallbacks(this);
}

StyleEngine::~StyleEngine()
{
}

static bool isStyleElement(Node& node)
{
    return isHTMLStyleElement(node) || isSVGStyleElement(node);
}

#if !ENABLE(OILPAN)
void StyleEngine::detachFromDocument()
{
    // Cleanup is performed eagerly when the StyleEngine is removed from the
    // document. The StyleEngine is unreachable after this, since only the
    // document has a reference to it.
    for (unsigned i = 0; i < m_injectedAuthorStyleSheets.size(); ++i)
        m_injectedAuthorStyleSheets[i]->clearOwnerNode();
    for (unsigned i = 0; i < m_authorStyleSheets.size(); ++i)
        m_authorStyleSheets[i]->clearOwnerNode();

    if (m_fontSelector) {
        m_fontSelector->clearDocument();
        m_fontSelector->unregisterForInvalidationCallbacks(this);
    }

    // Decrement reference counts for things we could be keeping alive.
    m_fontSelector.clear();
    m_resolver.clear();
    m_styleSheetCollectionMap.clear();
    m_activeTreeScopes.clear();
}
#endif

inline Document* StyleEngine::master()
{
    if (isMaster())
        return m_document;
    HTMLImportsController* import = document().importsController();
    if (!import) // Document::import() can return null while executing its destructor.
        return 0;
    return import->master();
}

void StyleEngine::OrderedTreeScopeSet::insert(TreeScope* treeScope)
{
    if (m_treeScopes.isEmpty()) {
        m_treeScopes.append(treeScope);
        m_hash.add(treeScope);
        return;
    }
    if (m_hash.contains(treeScope))
        return;

    int end = m_treeScopes.size() - 1;
    int start = 0;
    int position = 0;
    unsigned result = 0;

    while (start <= end) {
        position = (start + end) / 2;
        result = m_treeScopes[position]->comparePosition(*treeScope);

        if (result & Node::DOCUMENT_POSITION_PRECEDING) {
            end = position - 1;
        } else {
            ASSERT(result & Node::DOCUMENT_POSITION_FOLLOWING);
            start = position + 1;
        }
    }

    if (result & Node::DOCUMENT_POSITION_FOLLOWING) {
        ++position;
        ASSERT(static_cast<size_t>(position) == m_treeScopes.size() || (m_treeScopes[position]->comparePosition(*treeScope) & Node::DOCUMENT_POSITION_PRECEDING));
    }
    m_treeScopes.insert(position, treeScope);
    m_hash.add(treeScope);

#if ENABLE(ASSERT)
    // Check whether m_treeScopes is sorted in document order or not.
    for (unsigned i = 0; i < m_treeScopes.size() - 1; ++i) {
        unsigned result = m_treeScopes[i]->comparePosition(*m_treeScopes[i + 1]);
        ASSERT(result & Node::DOCUMENT_POSITION_FOLLOWING);
    }
#endif
}

void StyleEngine::OrderedTreeScopeSet::remove(TreeScope* treeScope)
{
    if (!m_hash.contains(treeScope))
        return;
    size_t position = m_treeScopes.find(treeScope);
    m_treeScopes.remove(position);
    m_hash.remove(treeScope);
}

DEFINE_TRACE(StyleEngine::OrderedTreeScopeSet)
{
#if ENABLE(OILPAN)
    visitor->trace(m_treeScopes);
    visitor->trace(m_hash);
#endif
}

TreeScopeStyleSheetCollection* StyleEngine::ensureStyleSheetCollectionFor(TreeScope& treeScope)
{
    if (treeScope == m_document)
        return documentStyleSheetCollection();

    StyleSheetCollectionMap::AddResult result = m_styleSheetCollectionMap.add(&treeScope, nullptr);
    if (result.isNewEntry)
        result.storedValue->value = adoptPtrWillBeNoop(new ShadowTreeStyleSheetCollection(toShadowRoot(treeScope)));
    return result.storedValue->value.get();
}

TreeScopeStyleSheetCollection* StyleEngine::styleSheetCollectionFor(TreeScope& treeScope)
{
    if (treeScope == m_document)
        return documentStyleSheetCollection();

    StyleSheetCollectionMap::iterator it = m_styleSheetCollectionMap.find(&treeScope);
    if (it == m_styleSheetCollectionMap.end())
        return 0;
    return it->value.get();
}

const WillBeHeapVector<RefPtrWillBeMember<StyleSheet>>& StyleEngine::styleSheetsForStyleSheetList(TreeScope& treeScope)
{
    if (treeScope == m_document)
        return documentStyleSheetCollection()->styleSheetsForStyleSheetList();

    return ensureStyleSheetCollectionFor(treeScope)->styleSheetsForStyleSheetList();
}

const WillBeHeapVector<RefPtrWillBeMember<CSSStyleSheet>>& StyleEngine::activeAuthorStyleSheets() const
{
    return documentStyleSheetCollection()->activeAuthorStyleSheets();
}

void StyleEngine::combineCSSFeatureFlags(const RuleFeatureSet& features)
{
    // Delay resetting the flags until after next style recalc since unapplying the style may not work without these set (this is true at least with before/after).
    m_usesSiblingRules = m_usesSiblingRules || features.usesSiblingRules();
    m_usesFirstLineRules = m_usesFirstLineRules || features.usesFirstLineRules();
    m_usesWindowInactiveSelector = m_usesWindowInactiveSelector || features.usesWindowInactiveSelector();
    m_maxDirectAdjacentSelectors = max(m_maxDirectAdjacentSelectors, features.maxDirectAdjacentSelectors());
}

void StyleEngine::resetCSSFeatureFlags(const RuleFeatureSet& features)
{
    m_usesSiblingRules = features.usesSiblingRules();
    m_usesFirstLineRules = features.usesFirstLineRules();
    m_usesWindowInactiveSelector = features.usesWindowInactiveSelector();
    m_maxDirectAdjacentSelectors = features.maxDirectAdjacentSelectors();
}

const WillBeHeapVector<RefPtrWillBeMember<CSSStyleSheet>>& StyleEngine::injectedAuthorStyleSheets() const
{
    updateInjectedStyleSheetCache();
    return m_injectedAuthorStyleSheets;
}

void StyleEngine::updateInjectedStyleSheetCache() const
{
    if (m_injectedStyleSheetCacheValid)
        return;
    m_injectedStyleSheetCacheValid = true;
    m_injectedAuthorStyleSheets.clear();

    Page* owningPage = document().page();
    if (!owningPage)
        return;

    const InjectedStyleSheetEntryVector& entries = InjectedStyleSheets::instance().entries();
    for (unsigned i = 0; i < entries.size(); ++i) {
        const InjectedStyleSheetEntry* entry = entries[i].get();
        if (entry->injectedFrames() == InjectStyleInTopFrameOnly && document().ownerElement())
            continue;
        if (!URLPatternMatcher::matchesPatterns(document().url(), entry->whitelist()))
            continue;
        RefPtrWillBeRawPtr<CSSStyleSheet> groupSheet = CSSStyleSheet::createInline(m_document, KURL());
        m_injectedAuthorStyleSheets.append(groupSheet);
        groupSheet->contents()->parseString(entry->source());
    }
}

void StyleEngine::invalidateInjectedStyleSheetCache()
{
    m_injectedStyleSheetCacheValid = false;
    markDocumentDirty();
    // FIXME: updateInjectedStyleSheetCache is called inside StyleSheetCollection::updateActiveStyleSheets
    // and batch updates lots of sheets so we can't call addedStyleSheet() or removedStyleSheet().
    document().styleResolverChanged();
}

void StyleEngine::compatibilityModeChanged()
{
    if (!m_injectedAuthorStyleSheets.isEmpty())
        invalidateInjectedStyleSheetCache();
}

void StyleEngine::addAuthorSheet(PassRefPtrWillBeRawPtr<StyleSheetContents> authorSheet)
{
    m_authorStyleSheets.append(CSSStyleSheet::create(authorSheet, m_document));
    document().addedStyleSheet(m_authorStyleSheets.last().get());
    markDocumentDirty();
}

void StyleEngine::addPendingSheet()
{
    m_pendingStylesheets++;
}

// This method is called whenever a top-level stylesheet has finished loading.
void StyleEngine::removePendingSheet(Node* styleSheetCandidateNode)
{
    ASSERT(styleSheetCandidateNode);
    TreeScope* treeScope = isStyleElement(*styleSheetCandidateNode) ? &styleSheetCandidateNode->treeScope() : m_document.get();
    markTreeScopeDirty(*treeScope);

    // Make sure we knew this sheet was pending, and that our count isn't out of sync.
    ASSERT(m_pendingStylesheets > 0);

    m_pendingStylesheets--;
    if (m_pendingStylesheets)
        return;

    // FIXME: We can't call addedStyleSheet or removedStyleSheet here because we don't know
    // what's new. We should track that to tell the style system what changed.
    document().didRemoveAllPendingStylesheet();
}

void StyleEngine::modifiedStyleSheet(StyleSheet* sheet)
{
    if (!sheet)
        return;

    Node* node = sheet->ownerNode();
    if (!node || !node->inDocument())
        return;

    TreeScope& treeScope = isStyleElement(*node) ? node->treeScope() : *m_document;
    ASSERT(isStyleElement(*node) || treeScope == m_document);

    markTreeScopeDirty(treeScope);
}

void StyleEngine::addStyleSheetCandidateNode(Node* node, bool createdByParser)
{
    if (!node->inDocument() || document().isDetached())
        return;

    TreeScope& treeScope = isStyleElement(*node) ? node->treeScope() : *m_document;
    ASSERT(isStyleElement(*node) || treeScope == m_document);
    ASSERT(!isXSLStyleSheet(*node));
    TreeScopeStyleSheetCollection* collection = ensureStyleSheetCollectionFor(treeScope);
    ASSERT(collection);
    collection->addStyleSheetCandidateNode(node, createdByParser);

    markTreeScopeDirty(treeScope);
    if (treeScope != m_document)
        m_activeTreeScopes.insert(&treeScope);
}

void StyleEngine::removeStyleSheetCandidateNode(Node* node)
{
    removeStyleSheetCandidateNode(node, *m_document);
}

void StyleEngine::removeStyleSheetCandidateNode(Node* node, TreeScope& treeScope)
{
    ASSERT(isStyleElement(*node) || treeScope == m_document);
    ASSERT(!isXSLStyleSheet(*node));

    TreeScopeStyleSheetCollection* collection = styleSheetCollectionFor(treeScope);
    // After detaching document, collection could be null. In the case,
    // we should not update anything. Instead, just return.
    if (!collection)
        return;
    collection->removeStyleSheetCandidateNode(node);

    markTreeScopeDirty(treeScope);
}

void StyleEngine::modifiedStyleSheetCandidateNode(Node* node)
{
    if (!node->inDocument())
        return;

    TreeScope& treeScope = isStyleElement(*node) ? node->treeScope() : *m_document;
    ASSERT(isStyleElement(*node) || treeScope == m_document);
    markTreeScopeDirty(treeScope);
}

void StyleEngine::setExitTransitionStylesheetsEnabled(bool enabled)
{
    TreeScopeStyleSheetCollection* collection = ensureStyleSheetCollectionFor(*m_document);
    collection->setExitTransitionStyleshetsEnabled(enabled);
}

bool StyleEngine::shouldUpdateDocumentStyleSheetCollection(StyleResolverUpdateMode updateMode) const
{
    return m_documentScopeDirty || updateMode == FullStyleUpdate;
}

bool StyleEngine::shouldUpdateShadowTreeStyleSheetCollection(StyleResolverUpdateMode updateMode) const
{
    return !m_dirtyTreeScopes.isEmpty() || updateMode == FullStyleUpdate;
}

void StyleEngine::clearMediaQueryRuleSetOnTreeScopeStyleSheets(UnorderedTreeScopeSet::iterator begin, UnorderedTreeScopeSet::iterator end)
{
    for (UnorderedTreeScopeSet::iterator it = begin; it != end; ++it) {
        TreeScope& treeScope = **it;
        ASSERT(treeScope != m_document);
        ShadowTreeStyleSheetCollection* collection = static_cast<ShadowTreeStyleSheetCollection*>(styleSheetCollectionFor(treeScope));
        ASSERT(collection);
        collection->clearMediaQueryRuleSetStyleSheets();
    }
}

void StyleEngine::clearMediaQueryRuleSetStyleSheets()
{
    documentStyleSheetCollection()->clearMediaQueryRuleSetStyleSheets();
    clearMediaQueryRuleSetOnTreeScopeStyleSheets(m_activeTreeScopes.beginUnordered(), m_activeTreeScopes.endUnordered());
    clearMediaQueryRuleSetOnTreeScopeStyleSheets(m_dirtyTreeScopes.begin(), m_dirtyTreeScopes.end());
}

void StyleEngine::updateStyleSheetsInImport(DocumentStyleSheetCollector& parentCollector)
{
    ASSERT(!isMaster());
    WillBeHeapVector<RefPtrWillBeMember<StyleSheet>> sheetsForList;
    ImportedDocumentStyleSheetCollector subcollector(parentCollector, sheetsForList);
    documentStyleSheetCollection()->collectStyleSheets(*this, subcollector);
    documentStyleSheetCollection()->swapSheetsForSheetList(sheetsForList);
}

void StyleEngine::updateActiveStyleSheetsInShadow(StyleResolverUpdateMode updateMode, TreeScope* treeScope, UnorderedTreeScopeSet& treeScopesRemoved)
{
    ASSERT(treeScope != m_document);
    ShadowTreeStyleSheetCollection* collection = static_cast<ShadowTreeStyleSheetCollection*>(styleSheetCollectionFor(*treeScope));
    ASSERT(collection);
    collection->updateActiveStyleSheets(*this, updateMode);
    if (!collection->hasStyleSheetCandidateNodes()) {
        treeScopesRemoved.add(treeScope);
        // When removing TreeScope from ActiveTreeScopes,
        // its resolver should be destroyed by invoking resetAuthorStyle.
        ASSERT(!treeScope->scopedStyleResolver());
    }
}

void StyleEngine::updateActiveStyleSheets(StyleResolverUpdateMode updateMode)
{
    ASSERT(isMaster());
    ASSERT(!document().inStyleRecalc());

    if (!document().isActive())
        return;

    if (shouldUpdateDocumentStyleSheetCollection(updateMode))
        documentStyleSheetCollection()->updateActiveStyleSheets(*this, updateMode);

    if (shouldUpdateShadowTreeStyleSheetCollection(updateMode)) {
        UnorderedTreeScopeSet treeScopesRemoved;

        if (updateMode == FullStyleUpdate) {
            for (unsigned i = 0; i < m_activeTreeScopes.size(); ++i)
                updateActiveStyleSheetsInShadow(updateMode, m_activeTreeScopes[i], treeScopesRemoved);
        } else {
            for (UnorderedTreeScopeSet::iterator it = m_dirtyTreeScopes.begin(); it != m_dirtyTreeScopes.end(); ++it) {
                updateActiveStyleSheetsInShadow(updateMode, *it, treeScopesRemoved);
            }
        }
        for (UnorderedTreeScopeSet::iterator it = treeScopesRemoved.begin(); it != treeScopesRemoved.end(); ++it)
            m_activeTreeScopes.remove(*it);
    }

    InspectorInstrumentation::activeStyleSheetsUpdated(m_document);
    m_usesRemUnits = documentStyleSheetCollection()->usesRemUnits();

    m_dirtyTreeScopes.clear();
    m_documentScopeDirty = false;
}

const WillBeHeapVector<RefPtrWillBeMember<CSSStyleSheet>> StyleEngine::activeStyleSheetsForInspector() const
{
    if (m_activeTreeScopes.isEmpty())
        return documentStyleSheetCollection()->activeAuthorStyleSheets();

    WillBeHeapVector<RefPtrWillBeMember<CSSStyleSheet>> activeStyleSheets;

    activeStyleSheets.appendVector(documentStyleSheetCollection()->activeAuthorStyleSheets());
    for (unsigned i = 0; i < m_activeTreeScopes.size(); ++i) {
        TreeScope* treeScope = const_cast<TreeScope*>(m_activeTreeScopes[i]);
        if (TreeScopeStyleSheetCollection* collection = m_styleSheetCollectionMap.get(treeScope))
            activeStyleSheets.appendVector(collection->activeAuthorStyleSheets());
    }

    // FIXME: Inspector needs a vector which has all active stylesheets.
    // However, creating such a large vector might cause performance regression.
    // Need to implement some smarter solution.
    return activeStyleSheets;
}

void StyleEngine::didRemoveShadowRoot(ShadowRoot* shadowRoot)
{
    m_styleSheetCollectionMap.remove(shadowRoot);
    m_activeTreeScopes.remove(shadowRoot);
    m_dirtyTreeScopes.remove(shadowRoot);
}

void StyleEngine::shadowRootRemovedFromDocument(ShadowRoot* shadowRoot)
{
    if (StyleResolver* styleResolver = resolver()) {
        styleResolver->resetAuthorStyle(*shadowRoot);

        if (TreeScopeStyleSheetCollection* collection = styleSheetCollectionFor(*shadowRoot))
            styleResolver->removePendingAuthorStyleSheets(collection->activeAuthorStyleSheets());
    }
    m_styleSheetCollectionMap.remove(shadowRoot);
    m_activeTreeScopes.remove(shadowRoot);
    m_dirtyTreeScopes.remove(shadowRoot);
}

void StyleEngine::appendActiveAuthorStyleSheets()
{
    ASSERT(isMaster());

    m_resolver->appendAuthorStyleSheets(documentStyleSheetCollection()->activeAuthorStyleSheets());
    for (unsigned i = 0; i < m_activeTreeScopes.size(); ++i) {
        if (TreeScopeStyleSheetCollection* collection = m_styleSheetCollectionMap.get(m_activeTreeScopes[i]))
            m_resolver->appendAuthorStyleSheets(collection->activeAuthorStyleSheets());
    }
    m_resolver->finishAppendAuthorStyleSheets();
}

void StyleEngine::createResolver()
{
    // It is a programming error to attempt to resolve style on a Document
    // which is not in a frame. Code which hits this should have checked
    // Document::isActive() before calling into code which could get here.

    ASSERT(document().frame());

    m_resolver = adoptPtrWillBeNoop(new StyleResolver(*m_document));

    // A scoped style resolver for document will be created during
    // appendActiveAuthorStyleSheets if needed.
    appendActiveAuthorStyleSheets();
    combineCSSFeatureFlags(m_resolver->ensureUpdatedRuleFeatureSet());
}

void StyleEngine::clearResolver()
{
    ASSERT(!document().inStyleRecalc());
    ASSERT(isMaster() || !m_resolver);

    document().clearScopedStyleResolver();
    // StyleEngine::shadowRootRemovedFromDocument removes not-in-document
    // treescopes from activeTreeScopes. StyleEngine::didRemoveShadowRoot
    // removes treescopes which are being destroyed from activeTreeScopes.
    // So we need to clearScopedStyleResolver for treescopes which have been
    // just removed from document. If document is destroyed before invoking
    // updateActiveStyleSheets, the treescope has a scopedStyleResolver which
    // has destroyed StyleSheetContents.
    for (UnorderedTreeScopeSet::iterator it = m_activeTreeScopes.beginUnordered(); it != m_activeTreeScopes.endUnordered(); ++it)
        (*it)->clearScopedStyleResolver();

    if (m_resolver)
        document().updateStyleInvalidationIfNeeded();
    m_resolver.clear();
}

void StyleEngine::clearMasterResolver()
{
    if (Document* master = this->master())
        master->styleEngine().clearResolver();
}

unsigned StyleEngine::resolverAccessCount() const
{
    return m_resolver ? m_resolver->accessCount() : 0;
}

void StyleEngine::didDetach()
{
    clearResolver();
}

bool StyleEngine::shouldClearResolver() const
{
    return !m_didCalculateResolver && !haveStylesheetsLoaded();
}

void StyleEngine::resolverChanged(StyleResolverUpdateMode mode)
{
    if (!isMaster()) {
        if (Document* master = this->master())
            master->styleResolverChanged(mode);
        return;
    }

    // Don't bother updating, since we haven't loaded all our style info yet
    // and haven't calculated the style selector for the first time.
    if (!document().isActive() || shouldClearResolver()) {
        clearResolver();
        return;
    }

    m_didCalculateResolver = true;
    updateActiveStyleSheets(mode);
}

void StyleEngine::clearFontCache()
{
    if (m_fontSelector)
        m_fontSelector->fontFaceCache()->clearCSSConnected();
    if (m_resolver)
        m_resolver->invalidateMatchedPropertiesCache();
}

void StyleEngine::updateGenericFontFamilySettings()
{
    // FIXME: we should not update generic font family settings when
    // document is inactive.
    ASSERT(document().isActive());

    if (!m_fontSelector)
        return;

    m_fontSelector->updateGenericFontFamilySettings(*m_document);
    if (m_resolver)
        m_resolver->invalidateMatchedPropertiesCache();
}

void StyleEngine::removeFontFaceRules(const WillBeHeapVector<RawPtrWillBeMember<const StyleRuleFontFace>>& fontFaceRules)
{
    if (!m_fontSelector)
        return;

    FontFaceCache* cache = m_fontSelector->fontFaceCache();
    for (unsigned i = 0; i < fontFaceRules.size(); ++i)
        cache->remove(fontFaceRules[i]);
    if (m_resolver)
        m_resolver->invalidateMatchedPropertiesCache();
}

void StyleEngine::markTreeScopeDirty(TreeScope& scope)
{
    if (scope == m_document) {
        markDocumentDirty();
        return;
    }

    m_dirtyTreeScopes.add(&scope);
}

void StyleEngine::markDocumentDirty()
{
    m_documentScopeDirty = true;
    if (document().importLoader())
        document().importsController()->master()->styleEngine().markDocumentDirty();
}

static bool isCacheableForStyleElement(const StyleSheetContents& contents)
{
    // FIXME: Support copying import rules.
    if (!contents.importRules().isEmpty())
        return false;
    // Until import rules are supported in cached sheets it's not possible for loading to fail.
    ASSERT(!contents.didLoadErrorOccur());
    // It is not the original sheet anymore.
    if (contents.isMutable())
        return false;
    if (!contents.hasSyntacticallyValidCSSHeader())
        return false;
    return true;
}

PassRefPtrWillBeRawPtr<CSSStyleSheet> StyleEngine::createSheet(Element* e, const String& text, TextPosition startPosition, bool createdByParser)
{
    RefPtrWillBeRawPtr<CSSStyleSheet> styleSheet = nullptr;

    e->document().styleEngine().addPendingSheet();

    if (!e->document().inQuirksMode()) {
        AtomicString textContent(text);

        WillBeHeapHashMap<AtomicString, RawPtrWillBeMember<StyleSheetContents>>::AddResult result = m_textToSheetCache.add(textContent, nullptr);
        if (result.isNewEntry || !result.storedValue->value) {
            styleSheet = StyleEngine::parseSheet(e, text, startPosition, createdByParser);
            if (result.isNewEntry && isCacheableForStyleElement(*styleSheet->contents())) {
                result.storedValue->value = styleSheet->contents();
                m_sheetToTextCache.add(styleSheet->contents(), textContent);
            }
        } else {
            StyleSheetContents* contents = result.storedValue->value;
            ASSERT(contents);
            ASSERT(isCacheableForStyleElement(*contents));
            ASSERT(contents->singleOwnerDocument() == e->document());
            styleSheet = CSSStyleSheet::createInline(contents, e, startPosition);
        }
    } else {
        // FIXME: currently we don't cache StyleSheetContents inQuirksMode.
        styleSheet = StyleEngine::parseSheet(e, text, startPosition, createdByParser);
    }

    ASSERT(styleSheet);
    styleSheet->setTitle(e->title());
    return styleSheet;
}

PassRefPtrWillBeRawPtr<CSSStyleSheet> StyleEngine::parseSheet(Element* e, const String& text, TextPosition startPosition, bool createdByParser)
{
    RefPtrWillBeRawPtr<CSSStyleSheet> styleSheet = nullptr;
    styleSheet = CSSStyleSheet::createInline(e, KURL(), startPosition, e->document().characterSet());
    styleSheet->contents()->parseStringAtPosition(text, startPosition, createdByParser);
    return styleSheet;
}

void StyleEngine::removeSheet(StyleSheetContents* contents)
{
    WillBeHeapHashMap<RawPtrWillBeMember<StyleSheetContents>, AtomicString>::iterator it = m_sheetToTextCache.find(contents);
    if (it == m_sheetToTextCache.end())
        return;

    m_textToSheetCache.remove(it->value);
    m_sheetToTextCache.remove(contents);
}

void StyleEngine::collectScopedStyleFeaturesTo(RuleFeatureSet& features) const
{
    HashSet<const StyleSheetContents*> visitedSharedStyleSheetContents;
    if (document().scopedStyleResolver())
        document().scopedStyleResolver()->collectFeaturesTo(features, visitedSharedStyleSheetContents);
    for (unsigned i = 0; i < m_activeTreeScopes.size(); ++i) {
        TreeScope* treeScope = const_cast<TreeScope*>(m_activeTreeScopes[i]);
        // When creating StyleResolver, dirty treescopes might not be processed.
        // So some active treescopes might not have a scoped style resolver.
        // In this case, we should skip collectFeatures for the treescopes without
        // scoped style resolvers. When invoking updateActiveStyleSheets,
        // the treescope's features will be processed.
        if (ScopedStyleResolver* resolver = treeScope->scopedStyleResolver())
            resolver->collectFeaturesTo(features, visitedSharedStyleSheetContents);
    }
}

void StyleEngine::fontsNeedUpdate(CSSFontSelector*)
{
    if (!document().isActive())
        return;

    if (m_resolver)
        m_resolver->invalidateMatchedPropertiesCache();
    document().setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::Fonts));
}

void StyleEngine::setFontSelector(PassRefPtrWillBeRawPtr<CSSFontSelector> fontSelector)
{
    m_fontSelector = fontSelector;
}

void StyleEngine::platformColorsChanged()
{
    if (m_resolver)
        m_resolver->invalidateMatchedPropertiesCache();
    document().setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::PlatformColorChange));
}

DEFINE_TRACE(StyleEngine)
{
#if ENABLE(OILPAN)
    visitor->trace(m_document);
    visitor->trace(m_injectedAuthorStyleSheets);
    visitor->trace(m_authorStyleSheets);
    visitor->trace(m_documentStyleSheetCollection);
    visitor->trace(m_styleSheetCollectionMap);
    visitor->trace(m_resolver);
    visitor->trace(m_dirtyTreeScopes);
    visitor->trace(m_activeTreeScopes);
    visitor->trace(m_fontSelector);
    visitor->trace(m_textToSheetCache);
    visitor->trace(m_sheetToTextCache);
#endif
    CSSFontSelectorClient::trace(visitor);
}

}
