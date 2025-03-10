/*
    Copyright (C) 1998 Lars Knoll (knoll@mpi-hd.mpg.de)
    Copyright (C) 2001 Dirk Mueller <mueller@kde.org>
    Copyright (C) 2006 Samuel Weinig (sam.weinig@gmail.com)
    Copyright (C) 2004, 2005, 2006, 2007 Apple Inc. All rights reserved.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef ImageResource_h
#define ImageResource_h

#include "core/CoreExport.h"
#include "core/fetch/ResourcePtr.h"
#include "platform/geometry/IntRect.h"
#include "platform/geometry/IntSizeHash.h"
#include "platform/geometry/LayoutSize.h"
#include "platform/graphics/ImageObserver.h"
#include "wtf/HashMap.h"

namespace blink {

class ImageResourceClient;
class ResourceFetcher;
class FloatSize;
class Length;
class MemoryCache;
class LayoutObject;
class SecurityOrigin;
class SVGImageForContainer;

class CORE_EXPORT ImageResource final : public Resource, public ImageObserver {
    friend class MemoryCache;

public:
    typedef ImageResourceClient ClientType;

    ImageResource(const ResourceRequest&);
    ImageResource(blink::Image*);
    // Exposed for testing
    ImageResource(const ResourceRequest&, blink::Image*);
    virtual ~ImageResource();

    virtual void load(ResourceFetcher*, const ResourceLoaderOptions&) override;

    blink::Image* image(); // Returns the nullImage() if the image is not available yet.
    blink::Image* imageForLayoutObject(const LayoutObject*); // Returns the nullImage() if the image is not available yet.
    bool hasImage() const { return m_image.get(); }
    // Side effect: ensures decoded image is in cache, therefore should only be called when about to draw the image.
    // FIXME: Decoding image on the main thread is expensive, so rather than forcing decode, consider returning false
    // when image is not decoded yet, as we do in case of deferred decoding.
    bool currentFrameKnownToBeOpaque(const LayoutObject*);

    static std::pair<blink::Image*, float> brokenImage(float deviceScaleFactor); // Returns an image and the image's resolution scale factor.
    bool willPaintBrokenImage() const;

    bool canRender(const LayoutObject& layoutObject, float multiplier) { return !errorOccurred() && !imageSizeForLayoutObject(&layoutObject, multiplier).isEmpty(); }

    void setContainerSizeForLayoutObject(const ImageResourceClient*, const IntSize&, float);
    bool usesImageContainerSize() const;
    bool imageHasRelativeWidth() const;
    bool imageHasRelativeHeight() const;
    // The device pixel ratio we got from the server for this image, or 1.0.
    float devicePixelRatioHeaderValue() const { return m_devicePixelRatioHeaderValue; }
    bool hasDevicePixelRatioHeaderValue() const { return m_hasDevicePixelRatioHeaderValue; }

    enum SizeType {
        NormalSize, // Report the size of the image associated with a certain layoutObject
        IntrinsicSize // Report the intrinsic size, i.e. ignore whatever has been set extrinsically.
    };
    // This method takes a zoom multiplier that can be used to increase the natural size of the image by the zoom.
    LayoutSize imageSizeForLayoutObject(const LayoutObject*, float multiplier, SizeType = NormalSize); // returns the size of the complete image.
    void computeIntrinsicDimensions(Length& intrinsicWidth, Length& intrinsicHeight, FloatSize& intrinsicRatio);

    bool isAccessAllowed(SecurityOrigin*);

    void updateImageAnimationPolicy();

    virtual void didAddClient(ResourceClient*) override;
    virtual void didRemoveClient(ResourceClient*) override;

    virtual void allClientsRemoved() override;

    virtual void appendData(const char*, unsigned) override;
    virtual void error(Resource::Status) override;
    virtual void responseReceived(const ResourceResponse&, PassOwnPtr<WebDataConsumerHandle>) override;
    virtual void finishOnePart() override;

    // For compatibility, images keep loading even if there are HTTP errors.
    virtual bool shouldIgnoreHTTPStatusCodeErrors() const override { return true; }

    virtual bool isImage() const override { return true; }
    virtual bool stillNeedsLoad() const override { return !errorOccurred() && status() == Unknown && !isLoading(); }

    // ImageObserver
    virtual void decodedSizeChanged(const blink::Image*, int delta) override;
    virtual void didDraw(const blink::Image*) override;

    virtual bool shouldPauseAnimation(const blink::Image*) override;
    virtual void animationAdvanced(const blink::Image*) override;
    virtual void changedInRect(const blink::Image*, const IntRect&) override;

protected:
    virtual bool isSafeToUnlock() const override;
    virtual void destroyDecodedDataIfPossible() override;

private:
    void clear();

    void setCustomAcceptHeader();
    void createImage();
    void updateImage(bool allDataReceived);
    void clearImage();
    // If not null, changeRect is the changed part of the image.
    void notifyObservers(const IntRect* changeRect = nullptr);
    IntSize svgImageSizeForLayoutObject(const LayoutObject*) const;
    blink::Image* svgImageForLayoutObject(const LayoutObject*);

    virtual void switchClientsToRevalidatedResource() override;

    typedef pair<IntSize, float> SizeAndZoom;
    typedef HashMap<const ImageResourceClient*, SizeAndZoom> ContainerSizeRequests;
    ContainerSizeRequests m_pendingContainerSizeRequests;
    float m_devicePixelRatioHeaderValue;

    typedef HashMap<const ImageResourceClient*, RefPtr<SVGImageForContainer>> ImageForContainerMap;
    OwnPtr<ImageForContainerMap> m_imageForContainerMap;

    RefPtr<blink::Image> m_image;
    bool m_loadingMultipartContent;
    bool m_hasDevicePixelRatioHeaderValue;
};

DEFINE_RESOURCE_TYPE_CASTS(Image);

}

#endif
