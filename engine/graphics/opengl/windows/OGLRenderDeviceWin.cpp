// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if defined(_WIN32) && OUZEL_COMPILE_OPENGL

#include <system_error>
#include "GL/glcorearb.h"
#include "GL/glext.h"
#include "GL/wglext.h"
#include "OGLRenderDeviceWin.hpp"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "core/windows/NativeWindowWin.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace opengl
        {
            namespace
            {
                constexpr LPCWSTR TEMP_WINDOW_CLASS_NAME = L"TempWindow";

                LRESULT CALLBACK windowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
                {
                    return DefWindowProc(window, msg, wParam, lParam);
                }

                class TempContext final
                {
                public:
                    TempContext()
                    {
                        HINSTANCE instance = GetModuleHandleW(nullptr);
                        if (!instance)
                            throw std::system_error(GetLastError(), std::system_category(), "Failed to get module handle");

                        WNDCLASSW wc;
                        wc.style = CS_OWNDC;
                        wc.lpfnWndProc = windowProc;
                        wc.cbClsExtra = 0;
                        wc.cbWndExtra = 0;
                        wc.hInstance = instance;
                        wc.hIcon = 0;
                        wc.hCursor = 0;
                        wc.hbrBackground = 0;
                        wc.lpszMenuName = nullptr;
                        wc.lpszClassName = TEMP_WINDOW_CLASS_NAME;

                        windowClass = RegisterClassW(&wc);
                        if (!windowClass)
                            throw std::system_error(GetLastError(), std::system_category(), "Failed to register window class");

                        window = CreateWindowW(TEMP_WINDOW_CLASS_NAME, L"TempWindow", 0,
                                            CW_USEDEFAULT, CW_USEDEFAULT,
                                            CW_USEDEFAULT, CW_USEDEFAULT,
                                            0, 0, instance, 0);
                        if (!window)
                            throw std::system_error(GetLastError(), std::system_category(), "Failed to create window");

                        deviceContext = GetDC(window);
                        if (!deviceContext)
                            throw std::runtime_error("Failed to get device context");

                        PIXELFORMATDESCRIPTOR pixelFormatDesc;
                        pixelFormatDesc.nSize = sizeof(pixelFormatDesc);
                        pixelFormatDesc.nVersion = 1;
                        pixelFormatDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
                        pixelFormatDesc.iPixelType = PFD_TYPE_RGBA;
                        pixelFormatDesc.cColorBits = 24;
                        pixelFormatDesc.cRedBits = 0;
                        pixelFormatDesc.cRedShift = 0;
                        pixelFormatDesc.cGreenBits = 0;
                        pixelFormatDesc.cGreenShift = 0;
                        pixelFormatDesc.cBlueBits = 0;
                        pixelFormatDesc.cBlueShift = 0;
                        pixelFormatDesc.cAlphaBits = 0;
                        pixelFormatDesc.cAlphaShift = 0;
                        pixelFormatDesc.cAccumBits = 0;
                        pixelFormatDesc.cAccumRedBits = 0;
                        pixelFormatDesc.cAccumGreenBits = 0;
                        pixelFormatDesc.cAccumBlueBits = 0;
                        pixelFormatDesc.cAccumAlphaBits = 0;
                        pixelFormatDesc.cDepthBits = 0;
                        pixelFormatDesc.cStencilBits = 0;
                        pixelFormatDesc.cAuxBuffers = 0;
                        pixelFormatDesc.iLayerType = PFD_MAIN_PLANE;
                        pixelFormatDesc.bReserved = 0;
                        pixelFormatDesc.dwLayerMask = 0;
                        pixelFormatDesc.dwVisibleMask = 0;
                        pixelFormatDesc.dwDamageMask = 0;

                        const int pixelFormat = ChoosePixelFormat(deviceContext, &pixelFormatDesc);

                        if (!pixelFormat)
                            throw std::system_error(GetLastError(), std::system_category(), "Failed to choose pixel format");

                        if (!SetPixelFormat(deviceContext, pixelFormat, &pixelFormatDesc))
                            throw std::system_error(GetLastError(), std::system_category(), "Failed to set pixel format");

                        renderContext = wglCreateContext(deviceContext);
                        if (!renderContext)
                            throw std::system_error(GetLastError(), std::system_category(), "Failed to create OpenGL rendering context");

                        if (!wglMakeCurrent(deviceContext, renderContext))
                            throw std::system_error(GetLastError(), std::system_category(), "Failed to set OpenGL rendering context");
                    }

                    ~TempContext()
                    {
                        if (renderContext)
                        {
                            if (wglGetCurrentContext() == renderContext)
                                wglMakeCurrent(deviceContext, nullptr);
                            wglDeleteContext(renderContext);
                        }

                        if (window)
                        {
                            if (deviceContext)
                                ReleaseDC(window, deviceContext);

                            DestroyWindow(window);
                        }

                        if (windowClass)
                            UnregisterClassW(TEMP_WINDOW_CLASS_NAME, GetModuleHandleW(nullptr));
                    }

                    TempContext(const TempContext&) = delete;
                    TempContext& operator=(const TempContext&) = delete;
                    TempContext(TempContext&&) = delete;
                    TempContext& operator=(TempContext&&) = delete;

                private:
                    ATOM windowClass = 0;
                    HWND window = 0;
                    HDC deviceContext = 0;
                    HGLRC renderContext = 0;
                };

                ApiVersion getVersion()
                {
                    HMODULE module = LoadLibraryW(L"opengl32.dll");
                    if (!module)
                        throw std::system_error(GetLastError(), std::system_category(), "Failed to load opengl32.dll");

                    PFNGLGETSTRINGPROC glGetStringProc = reinterpret_cast<PFNGLGETSTRINGPROC>(GetProcAddress(module, "glGetString"));

                    const GLubyte* versionPtr = glGetStringProc(GL_VERSION);

                    if (!versionPtr)
                        throw std::runtime_error("Failed to get OpenGL version");

                    const std::string versionStr(reinterpret_cast<const char*>(versionPtr));
                    std::string versionParts[2];
                    std::uint32_t part = 0;

                    for (const auto c : versionStr)
                        if (c != '.')
                            versionParts[part] += c;
                        else if (++part > 1)
                            break;

                    FreeLibrary(module);

                    return ApiVersion(static_cast<std::uint16_t>(std::stoi(versionParts[0])),
                                      static_cast<std::uint16_t>(std::stoi(versionParts[1])));
                }
            }

            RenderDeviceWin::RenderDeviceWin(const std::function<void(const Event&)>& initCallback):
                RenderDevice(initCallback)
            {
            }

            RenderDeviceWin::~RenderDeviceWin()
            {
                running = false;
                CommandBuffer commandBuffer;
                commandBuffer.pushCommand(std::make_unique<PresentCommand>());
                submitCommandBuffer(std::move(commandBuffer));

                if (renderThread.isJoinable()) renderThread.join();

                if (renderContext)
                {
                    if (wglGetCurrentContext() == renderContext)
                        wglMakeCurrent(deviceContext, nullptr);
                    wglDeleteContext(renderContext);
                }

                if (deviceContext)
                {
                    NativeWindowWin* windowWin = static_cast<NativeWindowWin*>(window->getNativeWindow());
                    ReleaseDC(windowWin->getNativeWindow(), deviceContext);
                }
            }

            void RenderDeviceWin::init(Window* newWindow,
                                       const Size2U& newSize,
                                       std::uint32_t newSampleCount,
                                       SamplerFilter newTextureFilter,
                                       std::uint32_t newMaxAnisotropy,
                                       bool newSrgb,
                                       bool newVerticalSync,
                                       bool newDepth,
                                       bool newStencil,
                                       bool newDebugRenderer)
            {
                TempContext tempContext;

                NativeWindowWin* windowWin = static_cast<NativeWindowWin*>(newWindow->getNativeWindow());

                deviceContext = GetDC(windowWin->getNativeWindow());
                if (!deviceContext)
                    throw std::runtime_error("Failed to get window's device context");

                std::vector<std::string> extensions;

                if (auto wglGetExtensionsStringProc = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGARBPROC>(wglGetProcAddress("wglGetExtensionsStringARB")))
                {
                    if (const char* extensionsPtr = wglGetExtensionsStringProc(deviceContext))
                        extensions = explodeString(std::string(extensionsPtr), ' ');

                    engine->log(Log::Level::All) << "Supported WGL extensions: " << extensions;
                }

                PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatProc = nullptr;
                PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsProc = nullptr;

                for (const auto& extension : extensions)
                {
                    if (extension == "WGL_ARB_pixel_format")
                        wglChoosePixelFormatProc = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));
                    else if (extension == "WGL_ARB_create_context")
                        wglCreateContextAttribsProc = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
                }

                int pixelFormat = 0;

                PIXELFORMATDESCRIPTOR pixelFormatDesc;
                pixelFormatDesc.nSize = sizeof(pixelFormatDesc);
                pixelFormatDesc.nVersion = 1;
                pixelFormatDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_GENERIC_ACCELERATED;
                pixelFormatDesc.iPixelType = PFD_TYPE_RGBA;
                pixelFormatDesc.cColorBits = 24;
                pixelFormatDesc.cRedBits = 0;
                pixelFormatDesc.cRedShift = 0;
                pixelFormatDesc.cGreenBits = 0;
                pixelFormatDesc.cGreenShift = 0;
                pixelFormatDesc.cBlueBits = 0;
                pixelFormatDesc.cBlueShift = 0;
                pixelFormatDesc.cAlphaBits = 0;
                pixelFormatDesc.cAlphaShift = 0;
                pixelFormatDesc.cAccumBits = 0;
                pixelFormatDesc.cAccumRedBits = 0;
                pixelFormatDesc.cAccumGreenBits = 0;
                pixelFormatDesc.cAccumBlueBits = 0;
                pixelFormatDesc.cAccumAlphaBits = 0;
                pixelFormatDesc.cDepthBits = newDepth ? 24 : 0;
                pixelFormatDesc.cStencilBits = 0;
                pixelFormatDesc.cAuxBuffers = 0;
                pixelFormatDesc.iLayerType = PFD_MAIN_PLANE;
                pixelFormatDesc.bReserved = 0;
                pixelFormatDesc.dwLayerMask = 0;
                pixelFormatDesc.dwVisibleMask = 0;
                pixelFormatDesc.dwDamageMask = 0;

                if (wglChoosePixelFormatProc)
                {
                    const int attributeList[] =
                    {
                        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
                        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
                        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
                        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
                        WGL_COLOR_BITS_ARB, 24,
                        WGL_ALPHA_BITS_ARB, 8,
                        WGL_DEPTH_BITS_ARB, newDepth ? 24 : 0,
                        WGL_STENCIL_BITS_ARB, newStencil ? 8 : 0,
                        WGL_SAMPLE_BUFFERS_ARB, newSampleCount > 0 ? 1 : 0,
                        WGL_SAMPLES_ARB, static_cast<int>(newSampleCount),
                        WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, newSrgb ? 1 : 0,
                        0,
                    };

                    UINT numFormats;

                    if (!wglChoosePixelFormatProc(deviceContext, attributeList, nullptr, 1, &pixelFormat, &numFormats))
                        throw std::system_error(GetLastError(), std::system_category(), "Failed to choose pixel format");
                }
                else
                {
                    pixelFormat = ChoosePixelFormat(deviceContext, &pixelFormatDesc);
                    if (!pixelFormat)
                        throw std::system_error(GetLastError(), std::system_category(), "Failed to choose pixel format");
                }

                if (!SetPixelFormat(deviceContext, pixelFormat, &pixelFormatDesc))
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to set pixel format");

                if (wglCreateContextAttribsProc)
                {
                    for (int openGLVersion = 4; openGLVersion >= 2; --openGLVersion)
                    {
                        std::vector<int> contextAttribs = {
                            WGL_CONTEXT_MAJOR_VERSION_ARB, openGLVersion,
                            WGL_CONTEXT_MINOR_VERSION_ARB, 0,
                        };

                        if (newDebugRenderer)
                        {
                            contextAttribs.push_back(WGL_CONTEXT_FLAGS_ARB);
                            contextAttribs.push_back(WGL_CONTEXT_DEBUG_BIT_ARB);
                        }

                        contextAttribs.push_back(0);

                        renderContext = wglCreateContextAttribsProc(deviceContext, 0, contextAttribs.data());

                        if (renderContext)
                        {
                            engine->log(Log::Level::Info) << "OpenGL " << openGLVersion << " context created";
                            break;
                        }
                    }
                }
                else
                    renderContext = wglCreateContext(deviceContext);

                if (!renderContext)
                    throw std::runtime_error("Failed to create OpenGL rendering context");

                if (!wglMakeCurrent(deviceContext, renderContext))
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to set OpenGL rendering context");

                apiVersion = getVersion();

                if (apiVersion.v[0] < 2 || apiVersion.v[0] > 4)
                    throw std::runtime_error("Unsupported OpenGL version");

                RenderDevice::init(newWindow,
                                   newSize,
                                   newSampleCount,
                                   newTextureFilter,
                                   newMaxAnisotropy,
                                   newSrgb,
                                   newVerticalSync,
                                   newDepth,
                                   newStencil,
                                   newDebugRenderer);

                if (!wglMakeCurrent(deviceContext, nullptr))
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to unset OpenGL rendering context");

                running = true;
                renderThread = Thread(&RenderDeviceWin::renderMain, this);
            }

            void RenderDeviceWin::present()
            {
                if (!SwapBuffers(deviceContext))
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to swap buffers");
            }

            void RenderDeviceWin::renderMain()
            {
                Thread::setCurrentThreadName("Render");

                if (!wglMakeCurrent(deviceContext, renderContext))
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to set OpenGL rendering context");

                while (running)
                {
                    try
                    {
                        process();
                    }
                    catch (const std::exception& e)
                    {
                        engine->log(Log::Level::Error) << e.what();
                    }
                }

                if (!wglMakeCurrent(deviceContext, nullptr))
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to unset OpenGL rendering context");
            }
        } // namespace opengl
    } // namespace graphics
} // namespace ouzel

#endif
