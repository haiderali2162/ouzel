// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_WINDOW_HPP
#define OUZEL_CORE_WINDOW_HPP

#include <memory>
#include <string>
#include "core/NativeWindow.hpp"
#include "graphics/Renderer.hpp"
#include "math/Size.hpp"

namespace ouzel
{
    class Engine;

    class Window final
    {
    public:
        enum Flags
        {
            RESIZABLE = 0x01,
            FULLSCREEN = 0x02,
            EXCLUSIVE_FULLSCREEN = 0x04,
            HIGH_DPI = 0x08,
            DEPTH = 0x10
        };

        enum class Mode
        {
            WINDOWED,
            WINDOWED_FULLSCREEN,
            FULLSCREEN
        };

        Window(Engine& initEngine,
               const Size2U& newSize,
               uint32_t flags,
               const std::string& newTitle,
               graphics::Driver graphicsDriver);
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;

        inline NativeWindow* getNativeWindow() const { return nativeWindow.get(); }

        void close();
        void update();

        inline const Size2U& getSize() const { return size; }
        void setSize(const Size2U& newSize);

        inline const Size2U& getResolution() const { return resolution; }

        inline bool isResizable() const { return resizable; }

        void setFullscreen(bool newFullscreen);
        inline bool isFullscreen() const { return fullscreen; }

        inline bool isExclusiveFullscreen() const { return exclusiveFullscreen; }

        inline const std::string& getTitle() const { return title; }
        void setTitle(const std::string& newTitle);
        void bringToFront();
        void show();
        void hide();
        void minimize();
        void maximize();
        void restore();

        inline bool isVisible() const
        {
            return visible;
        }

        inline bool isMinimized() const
        {
            return minimized;
        }

        inline Vector2F convertWindowToNormalizedLocation(const Vector2F& position) const
        {
            return Vector2F(position.v[0] / size.v[0], position.v[1] / size.v[1]);
        }

        inline Vector2F convertNormalizedToWindowLocation(const Vector2F& position) const
        {
            return Vector2F(position.v[0] * size.v[0], position.v[1] * size.v[1]);
        }

    private:
        void eventCallback(const NativeWindow::Event& event);
        void handleEvent(const NativeWindow::Event& event);

        Engine& engine;
        std::unique_ptr<NativeWindow> nativeWindow;

        Size2U size;
        Size2U resolution;
        bool resizable = false;
        bool fullscreen = false;
        bool exclusiveFullscreen = false;
        bool highDpi = true;
        bool visible = false;
        bool minimized = false;
        uint32_t displayId = 0;

        std::string title;

        std::mutex eventQueueMutex;
        std::queue<NativeWindow::Event> eventQueue;
    };
}

#endif // OUZEL_CORE_WINDOW_HPP
