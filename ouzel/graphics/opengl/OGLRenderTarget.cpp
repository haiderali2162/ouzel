// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include <stdexcept>
#include "OGLRenderTarget.hpp"
#include "OGLRenderDevice.hpp"
#include "OGLTexture.hpp"

namespace ouzel
{
    namespace graphics
    {
        OGLRenderTarget::OGLRenderTarget(OGLRenderDevice& renderDeviceOGL):
            OGLRenderResource(renderDeviceOGL)
        {
            glGenFramebuffersProc(1, &frameBufferId);

            GLenum error;

            if ((error = glGetErrorProc()) != GL_NO_ERROR)
                throw std::system_error(makeErrorCode(error), "Failed to upload texture data");

            clearMask = GL_COLOR_BUFFER_BIT;
        }

        OGLRenderTarget::~OGLRenderTarget()
        {
            if (frameBufferId)
                renderDevice.deleteFrameBuffer(frameBufferId);
        }

        void OGLRenderTarget::reload()
        {
            for (OGLTexture* colorTexture : colorTextures)
                colorTexture->reload();
            if (depthTexture) depthTexture->reload();

            glGenFramebuffersProc(1, &frameBufferId);
        }

        void OGLRenderTarget::addColorTexture(OGLTexture* texture)
        {
            if (texture && colorTextures.insert(texture).second)
            {
                GLenum index = static_cast<GLenum>(colorTextures.size() - 1);
                renderDevice.bindFrameBuffer(frameBufferId);

                glFramebufferTexture2DProc(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, texture->getTextureId(), 0);
                //glFramebufferRenderbufferProc(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_RENDERBUFFER, texture->getBufferId());

                GLenum error;
                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set frame buffer's depth render buffer");
            }
        }

        void OGLRenderTarget::removeColorTexture(OGLTexture* texture)
        {
            auto i = colorTextures.find(texture);

            if (i != colorTextures.end())
            {
                colorTextures.erase(i);

                GLenum index = 0;
                for (OGLTexture* colorTexture : colorTextures)
                {
                    glFramebufferTexture2DProc(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, colorTexture->getTextureId(), 0);
                    ++index;
                }

                glFramebufferTexture2DProc(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, 0, 0);
            }
        }

        void OGLRenderTarget::setDepthTexture(OGLTexture* texture)
        {
            depthTexture = texture;

            if (texture)
            {
                renderDevice.bindFrameBuffer(frameBufferId);

                glFramebufferTexture2DProc(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->getTextureId(), 0);
                //glFramebufferRenderbufferProc(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, texture->getBufferId());

                GLenum error;
                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set frame buffer's depth render buffer");
            }
        }

        void OGLRenderTarget::setClearColorBuffer(bool clear)
        {
            if (clear)
                clearMask |= GL_COLOR_BUFFER_BIT;
            else
                clearMask &= ~static_cast<GLbitfield>(GL_COLOR_BUFFER_BIT);
        }

        void OGLRenderTarget::setClearDepthBuffer(bool clear)
        {
            if (clear)
                clearMask |= GL_DEPTH_BUFFER_BIT;
            else
                clearMask &= ~static_cast<GLbitfield>(GL_DEPTH_BUFFER_BIT);
        }

        void OGLRenderTarget::setClearColor(Color color)
        {
            frameBufferClearColor[0] = color.normR();
            frameBufferClearColor[1] = color.normG();
            frameBufferClearColor[2] = color.normB();
            frameBufferClearColor[3] = color.normA();
        }

        void OGLRenderTarget::setClearDepth(float newClearDepth)
        {
            clearDepth = newClearDepth;
        }
    } // namespace graphics
} // namespace ouzel

#endif
