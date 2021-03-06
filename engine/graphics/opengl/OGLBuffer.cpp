// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "OGLBuffer.hpp"
#include "OGLRenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace opengl
        {
            Buffer::Buffer(RenderDevice& initRenderDevice,
                           BufferType initType,
                           std::uint32_t initFlags,
                           const std::vector<std::uint8_t>& initData,
                           std::uint32_t initSize):
                RenderResource(initRenderDevice),
                type(initType),
                flags(initFlags),
                data(initData),
                size(static_cast<GLsizeiptr>(initSize))
            {
                createBuffer();

                if (size > 0)
                {
                    renderDevice.bindBuffer(bufferType, bufferId);

                    if (data.empty())
                        renderDevice.glBufferDataProc(bufferType, size, nullptr,
                                                      (flags & Flags::Dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
                    else
                        renderDevice.glBufferDataProc(bufferType, size, data.data(),
                                                      (flags & Flags::Dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                    GLenum error;

                    if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to create buffer");
                }
            }

            Buffer::~Buffer()
            {
                if (bufferId)
                    renderDevice.deleteBuffer(bufferId);
            }

            void Buffer::reload()
            {
                bufferId = 0;

                createBuffer();

                if (size > 0)
                {
                    renderDevice.bindBuffer(bufferType, bufferId);

                    if (data.empty())
                        renderDevice.glBufferDataProc(bufferType, size, nullptr,
                                                      (flags & Flags::Dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
                    else
                        renderDevice.glBufferDataProc(bufferType, size, data.data(),
                                                      (flags & Flags::Dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                    GLenum error;

                    if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to create buffer");
                }
            }

            void Buffer::setData(const std::vector<std::uint8_t>& newData)
            {
                if (!(flags & Flags::Dynamic))
                    throw std::runtime_error("Buffer is not dynamic");

                if (newData.empty())
                    throw std::invalid_argument("Data is empty");

                data = newData;

                if (!bufferId)
                    throw std::runtime_error("Buffer not initialized");

                renderDevice.bindBuffer(bufferType, bufferId);

                if (static_cast<GLsizeiptr>(data.size()) > size)
                {
                    size = static_cast<GLsizeiptr>(data.size());

                    renderDevice.glBufferDataProc(bufferType, size, data.data(), GL_DYNAMIC_DRAW);

                    GLenum error;

                    if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to create buffer");
                }
                else
                {
                    renderDevice.glBufferSubDataProc(bufferType, 0, static_cast<GLsizeiptr>(data.size()), data.data());

                    GLenum error;

                    if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to upload buffer");
                }
            }

            void Buffer::createBuffer()
            {
                renderDevice.glGenBuffersProc(1, &bufferId);

                GLenum error;

                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to create buffer");

                switch (type)
                {
                    case BufferType::Index:
                        bufferType = GL_ELEMENT_ARRAY_BUFFER;
                        break;
                    case BufferType::Vertex:
                        bufferType = GL_ARRAY_BUFFER;
                        break;
                    default:
                        throw std::runtime_error("Unsupported buffer type");
                }
            }
        } // namespace opengl
    } // namespace graphics
} // namespace ouzel

#endif
