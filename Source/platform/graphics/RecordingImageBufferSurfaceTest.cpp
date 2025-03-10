// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "platform/graphics/RecordingImageBufferSurface.h"

#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/ImageBuffer.h"
#include "platform/graphics/ImageBufferClient.h"
#include "platform/graphics/UnacceleratedImageBufferSurface.h"
#include "public/platform/Platform.h"
#include "public/platform/WebThread.h"
#include "public/platform/WebTraceLocation.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkPictureRecorder.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/RefPtr.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace blink;
using testing::Test;

namespace {

class FakeImageBufferClient : public ImageBufferClient, public WebThread::TaskObserver {
public:
    FakeImageBufferClient(ImageBuffer* imageBuffer)
        : m_isDirty(false)
        , m_imageBuffer(imageBuffer)
        , m_frameCount(0)
    { }

    virtual ~FakeImageBufferClient() { }

    // ImageBufferClient implementation
    virtual void notifySurfaceInvalid() override { }
    virtual bool isDirty() override { return m_isDirty; };
    virtual void didFinalizeFrame() override
    {
        if (m_isDirty) {
            Platform::current()->currentThread()->removeTaskObserver(this);
            m_isDirty = false;
        }
        ++m_frameCount;
    }

    // TaskObserver implementation
    virtual void willProcessTask() override { ASSERT_NOT_REACHED(); }
    virtual void didProcessTask() override
    {
        ASSERT_TRUE(m_isDirty);
        FloatRect dirtyRect(0, 0, 1, 1);
        m_imageBuffer->finalizeFrame(dirtyRect);
        ASSERT_FALSE(m_isDirty);
    }
    virtual void restoreCanvasMatrixClipStack() override { };

    void fakeDraw()
    {
        if (m_isDirty)
            return;
        m_isDirty = true;
        Platform::current()->currentThread()->addTaskObserver(this);
    }

    int frameCount() { return m_frameCount; }

private:
    bool m_isDirty;
    ImageBuffer* m_imageBuffer;
    int m_frameCount;
};

class MockSurfaceFactory : public RecordingImageBufferFallbackSurfaceFactory {
public:
    MockSurfaceFactory() : m_createSurfaceCount(0) { }

    virtual PassOwnPtr<ImageBufferSurface> createSurface(const IntSize& size, OpacityMode opacityMode)
    {
        m_createSurfaceCount++;
        return adoptPtr(new UnacceleratedImageBufferSurface(size, opacityMode));
    }

    virtual ~MockSurfaceFactory() { }

    int createSurfaceCount() { return m_createSurfaceCount; }

private:
    int m_createSurfaceCount;
};

} // unnamed namespace

class RecordingImageBufferSurfaceTest : public Test {
protected:
    RecordingImageBufferSurfaceTest()
    {
        OwnPtr<MockSurfaceFactory> surfaceFactory = adoptPtr(new MockSurfaceFactory());
        m_surfaceFactory = surfaceFactory.get();
        OwnPtr<RecordingImageBufferSurface> testSurface = adoptPtr(new RecordingImageBufferSurface(IntSize(10, 10), surfaceFactory.release()));
        m_testSurface = testSurface.get();
        // We create an ImageBuffer in order for the testSurface to be
        // properly initialized with a GraphicsContext
        m_imageBuffer = ImageBuffer::create(testSurface.release());
        EXPECT_FALSE(!m_imageBuffer);
        m_fakeImageBufferClient = adoptPtr(new FakeImageBufferClient(m_imageBuffer.get()));
        m_imageBuffer->setClient(m_fakeImageBufferClient.get());
    }

public:
    void testEmptyPicture()
    {
        m_testSurface->initializeCurrentFrame();
        RefPtr<SkPicture> picture = m_testSurface->getPicture();
        EXPECT_TRUE((bool)picture.get());
        EXPECT_EQ(1, m_fakeImageBufferClient->frameCount());
        expectDisplayListEnabled(true);
    }

    void testNoFallbackWithClear()
    {
        m_testSurface->initializeCurrentFrame();
        m_testSurface->willOverwriteCanvas();
        m_testSurface->getPicture();
        EXPECT_EQ(1, m_fakeImageBufferClient->frameCount());
        expectDisplayListEnabled(true);
    }

    void testNonAnimatedCanvasUpdate()
    {
        m_testSurface->initializeCurrentFrame();
        // acquire picture twice to simulate a static canvas: nothing drawn between updates
        m_fakeImageBufferClient->fakeDraw();
        m_testSurface->getPicture();
        m_testSurface->getPicture();
        EXPECT_EQ(2, m_fakeImageBufferClient->frameCount());
        expectDisplayListEnabled(true);
    }

    void testAnimatedWithoutClear()
    {
        m_testSurface->initializeCurrentFrame();
        m_fakeImageBufferClient->fakeDraw();
        m_testSurface->getPicture();
        EXPECT_EQ(1, m_fakeImageBufferClient->frameCount());
        EXPECT_EQ(0, m_surfaceFactory->createSurfaceCount());
        expectDisplayListEnabled(true); // first frame has an implicit clear
        m_fakeImageBufferClient->fakeDraw();
        m_testSurface->getPicture();
        EXPECT_EQ(2, m_fakeImageBufferClient->frameCount());
        expectDisplayListEnabled(false);
    }

    void testFrameFinalizedByTaskObserver1()
    {
        m_testSurface->initializeCurrentFrame();
        expectDisplayListEnabled(true);
        m_testSurface->getPicture();
        EXPECT_EQ(1, m_fakeImageBufferClient->frameCount());
        expectDisplayListEnabled(true);
        m_fakeImageBufferClient->fakeDraw();
        EXPECT_EQ(1, m_fakeImageBufferClient->frameCount());
        expectDisplayListEnabled(true);
        m_testSurface->getPicture();
        EXPECT_EQ(2, m_fakeImageBufferClient->frameCount());
        expectDisplayListEnabled(true);
        m_fakeImageBufferClient->fakeDraw();
        EXPECT_EQ(2, m_fakeImageBufferClient->frameCount());
        expectDisplayListEnabled(true);
        // Display list will be disabled only after exiting the runLoop
    }
    void testFrameFinalizedByTaskObserver2()
    {
        EXPECT_EQ(3, m_fakeImageBufferClient->frameCount());
        expectDisplayListEnabled(false);
        m_testSurface->getPicture();
        EXPECT_EQ(3, m_fakeImageBufferClient->frameCount());
        expectDisplayListEnabled(false);
        m_fakeImageBufferClient->fakeDraw();
        EXPECT_EQ(3, m_fakeImageBufferClient->frameCount());
        expectDisplayListEnabled(false);
    }

    void testAnimatedWithClear()
    {
        m_testSurface->initializeCurrentFrame();
        m_testSurface->getPicture();
        m_testSurface->willOverwriteCanvas();
        m_fakeImageBufferClient->fakeDraw();
        EXPECT_EQ(1, m_fakeImageBufferClient->frameCount());
        m_testSurface->getPicture();
        EXPECT_EQ(2, m_fakeImageBufferClient->frameCount());
        expectDisplayListEnabled(true);
        // clear after use
        m_fakeImageBufferClient->fakeDraw();
        m_testSurface->willOverwriteCanvas();
        EXPECT_EQ(2, m_fakeImageBufferClient->frameCount());
        m_testSurface->getPicture();
        EXPECT_EQ(3, m_fakeImageBufferClient->frameCount());
        expectDisplayListEnabled(true);
    }

    void testClearRect()
    {
        m_testSurface->initializeCurrentFrame();
        m_testSurface->getPicture();
        m_imageBuffer->context()->clearRect(FloatRect(FloatPoint(0, 0), FloatSize(m_testSurface->size())));
        m_fakeImageBufferClient->fakeDraw();
        EXPECT_EQ(1, m_fakeImageBufferClient->frameCount());
        m_testSurface->getPicture();
        EXPECT_EQ(2, m_fakeImageBufferClient->frameCount());
        expectDisplayListEnabled(true);
    }

    void expectDisplayListEnabled(bool displayListEnabled)
    {
        EXPECT_EQ(displayListEnabled, (bool)m_testSurface->m_currentFrame.get());
        EXPECT_EQ(!displayListEnabled, (bool)m_testSurface->m_fallbackSurface.get());
        int expectedSurfaceCreationCount = displayListEnabled ? 0 : 1;
        EXPECT_EQ(expectedSurfaceCreationCount, m_surfaceFactory->createSurfaceCount());
    }

private:
    MockSurfaceFactory* m_surfaceFactory;
    RecordingImageBufferSurface* m_testSurface;
    OwnPtr<FakeImageBufferClient> m_fakeImageBufferClient;
    OwnPtr<ImageBuffer> m_imageBuffer;
};

namespace {

// The following test helper class installs a mock platform that provides a mock WebThread
// for the current thread. The Mock thread is capable of queuing a single non-delayed task
// and registering a single task observer. The run loop exits immediately after running
// the single task.
class AutoInstallCurrentThreadPlatformMock {
public:
    AutoInstallCurrentThreadPlatformMock()
    {
        m_oldPlatform = Platform::current();
        Platform::initialize(&m_mockPlatform);
    }

    ~AutoInstallCurrentThreadPlatformMock()
    {
        Platform::initialize(m_oldPlatform);
    }

private:
    class CurrentThreadMock : public WebThread {
    public:
        CurrentThreadMock() : m_taskObserver(0), m_task(0) { }

        virtual ~CurrentThreadMock()
        {
            EXPECT_EQ((Task*)0, m_task);
        }

        virtual void postTask(const WebTraceLocation&, Task* task)
        {
            EXPECT_EQ((Task*)0, m_task);
            m_task = task;
        }

        virtual void postDelayedTask(const WebTraceLocation&, Task*, long long delayMs) override { ASSERT_NOT_REACHED(); };

        virtual bool isCurrentThread() const override { return true; }
        virtual PlatformThreadId threadId() const override
        {
            ASSERT_NOT_REACHED();
            return 0;
        }

        virtual void addTaskObserver(TaskObserver* taskObserver) override
        {
            EXPECT_EQ((TaskObserver*)0, m_taskObserver);
            m_taskObserver = taskObserver;
        }

        virtual void removeTaskObserver(TaskObserver* taskObserver) override
        {
            EXPECT_EQ(m_taskObserver, taskObserver);
            m_taskObserver = 0;
        }

        virtual void enterRunLoop() override
        {
            if (m_taskObserver)
                m_taskObserver->willProcessTask();
            if (m_task) {
                m_task->run();
                delete m_task;
                m_task = 0;
            }
            if (m_taskObserver)
                m_taskObserver->didProcessTask();
        }

        virtual void exitRunLoop() override { ASSERT_NOT_REACHED(); }

    private:
        TaskObserver* m_taskObserver;
        Task* m_task;
    };

    class CurrentThreadPlatformMock : public Platform {
    public:
        CurrentThreadPlatformMock() { }
        virtual void cryptographicallyRandomValues(unsigned char* buffer, size_t length) { ASSERT_NOT_REACHED(); }
        virtual WebThread* currentThread() override { return &m_currentThread; }
    private:
        CurrentThreadMock m_currentThread;
    };

    CurrentThreadPlatformMock m_mockPlatform;
    Platform* m_oldPlatform;
};


#define DEFINE_TEST_TASK_WRAPPER_CLASS(TEST_METHOD)                                               \
class TestWrapperTask_ ## TEST_METHOD : public WebThread::Task {                           \
    public:                                                                                       \
        TestWrapperTask_ ## TEST_METHOD(RecordingImageBufferSurfaceTest* test) : m_test(test) { } \
        virtual void run() override { m_test->TEST_METHOD(); }                                    \
    private:                                                                                      \
        RecordingImageBufferSurfaceTest* m_test;                                                  \
};

#define CALL_TEST_TASK_WRAPPER(TEST_METHOD)                                                               \
    {                                                                                                     \
        AutoInstallCurrentThreadPlatformMock ctpm;                                                        \
        Platform::current()->currentThread()->postTask(FROM_HERE, new TestWrapperTask_ ## TEST_METHOD(this)); \
        Platform::current()->currentThread()->enterRunLoop();                                      \
    }

TEST_F(RecordingImageBufferSurfaceTest, testEmptyPicture)
{
    testEmptyPicture();
}

TEST_F(RecordingImageBufferSurfaceTest, testNoFallbackWithClear)
{
    testNoFallbackWithClear();
}

DEFINE_TEST_TASK_WRAPPER_CLASS(testNonAnimatedCanvasUpdate)
TEST_F(RecordingImageBufferSurfaceTest, testNonAnimatedCanvasUpdate)
{
    CALL_TEST_TASK_WRAPPER(testNonAnimatedCanvasUpdate)
    expectDisplayListEnabled(true);
}

DEFINE_TEST_TASK_WRAPPER_CLASS(testAnimatedWithoutClear)
TEST_F(RecordingImageBufferSurfaceTest, testAnimatedWithoutClear)
{
    CALL_TEST_TASK_WRAPPER(testAnimatedWithoutClear)
    expectDisplayListEnabled(false);
}

DEFINE_TEST_TASK_WRAPPER_CLASS(testFrameFinalizedByTaskObserver1)
DEFINE_TEST_TASK_WRAPPER_CLASS(testFrameFinalizedByTaskObserver2)
TEST_F(RecordingImageBufferSurfaceTest, testFrameFinalizedByTaskObserver)
{
    CALL_TEST_TASK_WRAPPER(testFrameFinalizedByTaskObserver1)
    expectDisplayListEnabled(false);
    CALL_TEST_TASK_WRAPPER(testFrameFinalizedByTaskObserver2)
    expectDisplayListEnabled(false);
}

DEFINE_TEST_TASK_WRAPPER_CLASS(testAnimatedWithClear)
TEST_F(RecordingImageBufferSurfaceTest, testAnimatedWithClear)
{
    CALL_TEST_TASK_WRAPPER(testAnimatedWithClear)
    expectDisplayListEnabled(true);
}

DEFINE_TEST_TASK_WRAPPER_CLASS(testClearRect)
TEST_F(RecordingImageBufferSurfaceTest, testClearRect)
{
    CALL_TEST_TASK_WRAPPER(testClearRect);
    expectDisplayListEnabled(true);
}

} // namespace
