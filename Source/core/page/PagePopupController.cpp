/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
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
#include "core/page/PagePopupController.h"

#include "core/page/PagePopup.h"
#include "core/page/PagePopupClient.h"
#include "platform/text/PlatformLocale.h"
#include "public/platform/Platform.h"

namespace blink {

PagePopupController::PagePopupController(PagePopup& popup, PagePopupClient* client)
    : m_popup(popup)
    , m_popupClient(client)
{
    ASSERT(client);
}

PassRefPtrWillBeRawPtr<PagePopupController> PagePopupController::create(PagePopup& popup, PagePopupClient* client)
{
    return adoptRefWillBeNoop(new PagePopupController(popup, client));
}

void PagePopupController::setValueAndClosePopup(int numValue, const String& stringValue)
{
    if (m_popupClient)
        m_popupClient->setValueAndClosePopup(numValue, stringValue);
}

void PagePopupController::setValue(const String& value)
{
    if (m_popupClient)
        m_popupClient->setValue(value);
}

void PagePopupController::closePopup()
{
    if (m_popupClient)
        m_popupClient->closePopup();
}

void PagePopupController::selectFontsFromOwnerDocument(Document* targetDocument)
{
    if (!targetDocument || !m_popupClient)
        return;
    m_popupClient->selectFontsFromOwnerDocument(*targetDocument);
}

String PagePopupController::localizeNumberString(const String& numberString)
{
    if (m_popupClient)
        return m_popupClient->locale().convertToLocalizedNumber(numberString);
    return numberString;
}

String PagePopupController::formatMonth(int year, int zeroBaseMonth)
{
    if (!m_popupClient)
        return emptyString();
    DateComponents date;
    date.setMonthsSinceEpoch((year - 1970) * 12.0 + zeroBaseMonth);
    return m_popupClient->locale().formatDateTime(date, Locale::FormatTypeMedium);
}

String PagePopupController::formatShortMonth(int year, int zeroBaseMonth)
{
    if (!m_popupClient)
        return emptyString();
    DateComponents date;
    date.setMonthsSinceEpoch((year - 1970) * 12.0 + zeroBaseMonth);
    return m_popupClient->locale().formatDateTime(date, Locale::FormatTypeShort);
}

String PagePopupController::formatWeek(int year, int weekNumber, const String& localizedDateString)
{
    if (!m_popupClient)
        return emptyString();
    DateComponents week;
    bool setWeekResult = week.setWeek(year, weekNumber);
    ASSERT_UNUSED(setWeekResult, setWeekResult);
    String localizedWeek = m_popupClient->locale().formatDateTime(week);
    return m_popupClient->locale().queryString(WebLocalizedString::AXCalendarWeekDescription, localizedWeek, localizedDateString);
}

void PagePopupController::clearPagePopupClient()
{
    m_popupClient = nullptr;
}

void PagePopupController::histogramEnumeration(const String& name, int sample, int boundaryValue)
{
    blink::Platform::current()->histogramEnumeration(name.utf8().data(), sample, boundaryValue);
}

void PagePopupController::setWindowRect(int x, int y, int width, int height)
{
    m_popup.setWindowRect(IntRect(x, y, width, height));
}

}
