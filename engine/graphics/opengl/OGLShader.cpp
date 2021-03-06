// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "OGLShader.hpp"
#include "OGLRenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace opengl
        {
            Shader::Shader(RenderDevice& initRenderDevice,
                           const std::vector<std::uint8_t>& initFragmentShader,
                           const std::vector<std::uint8_t>& initVertexShader,
                           const std::set<Vertex::Attribute::Usage>& initVertexAttributes,
                           const std::vector<std::pair<std::string, DataType>>& initFragmentShaderConstantInfo,
                           const std::vector<std::pair<std::string, DataType>>& initVertexShaderConstantInfo,
                           const std::string&,
                           const std::string&):
                RenderResource(initRenderDevice),
                fragmentShaderData(initFragmentShader),
                vertexShaderData(initVertexShader),
                vertexAttributes(initVertexAttributes),
                fragmentShaderConstantInfo(initFragmentShaderConstantInfo),
                vertexShaderConstantInfo(initVertexShaderConstantInfo)
            {
                compileShader();
            }

            Shader::~Shader()
            {
                if (programId) renderDevice.deleteProgram(programId);
                if (vertexShaderId) renderDevice.glDeleteShaderProc(vertexShaderId);
                if (fragmentShaderId) renderDevice.glDeleteShaderProc(fragmentShaderId);
            }

            void Shader::reload()
            {
                fragmentShaderId = 0;
                vertexShaderId = 0;
                programId = 0;

                compileShader();
            }

            std::string Shader::getShaderMessage(GLuint shaderId) const
            {
                GLint logLength = 0;
                renderDevice.glGetShaderivProc(shaderId, GL_INFO_LOG_LENGTH, &logLength);

                if (logLength > 0)
                {
                    std::vector<char> logMessage(static_cast<std::size_t>(logLength));
                    renderDevice.glGetShaderInfoLogProc(shaderId, logLength, nullptr, logMessage.data());

                    return std::string(logMessage.data());
                }

                return std::string();
            }

            std::string Shader::getProgramMessage() const
            {
                GLint logLength = 0;
                renderDevice.glGetProgramivProc(programId, GL_INFO_LOG_LENGTH, &logLength);

                if (logLength > 0)
                {
                    std::vector<char> logMessage(static_cast<std::size_t>(logLength));
                    renderDevice.glGetProgramInfoLogProc(programId, logLength, nullptr, logMessage.data());

                    return std::string("Shader linking error: ") + logMessage.data();
                }

                return std::string();
            }

            void Shader::compileShader()
            {
                fragmentShaderId = renderDevice.glCreateShaderProc(GL_FRAGMENT_SHADER);

                const auto fragmentShaderBuffer = reinterpret_cast<const GLchar*>(fragmentShaderData.data());
                const auto fragmentShaderSize = static_cast<GLint>(fragmentShaderData.size());

                renderDevice.glShaderSourceProc(fragmentShaderId, 1, &fragmentShaderBuffer, &fragmentShaderSize);
                renderDevice.glCompileShaderProc(fragmentShaderId);

                GLint status;
                renderDevice.glGetShaderivProc(fragmentShaderId, GL_COMPILE_STATUS, &status);
                if (status == GL_FALSE)
                    throw std::runtime_error("Failed to compile pixel shader, error: " + getShaderMessage(fragmentShaderId));

                GLenum error;

                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to get shader compile status");

                vertexShaderId = renderDevice.glCreateShaderProc(GL_VERTEX_SHADER);

                auto vertexShaderBuffer = reinterpret_cast<const GLchar*>(vertexShaderData.data());
                auto vertexShaderSize = static_cast<GLint>(vertexShaderData.size());

                renderDevice.glShaderSourceProc(vertexShaderId, 1, &vertexShaderBuffer, &vertexShaderSize);
                renderDevice.glCompileShaderProc(vertexShaderId);

                renderDevice.glGetShaderivProc(vertexShaderId, GL_COMPILE_STATUS, &status);
                if (status == GL_FALSE)
                    throw std::runtime_error("Failed to compile vertex shader, error: " + getShaderMessage(vertexShaderId));

                programId = renderDevice.glCreateProgramProc();

                renderDevice.glAttachShaderProc(programId, vertexShaderId);
                renderDevice.glAttachShaderProc(programId, fragmentShaderId);

                GLuint index = 0;

                for (const auto& vertexAttribute : RenderDevice::VERTEX_ATTRIBUTES)
                {
                    if (vertexAttributes.find(vertexAttribute.usage) != vertexAttributes.end())
                    {
                        const GLchar* name;

                        switch (vertexAttribute.usage)
                        {
                            case Vertex::Attribute::Usage::Binormal:
                                name = "binormal0";
                                break;
                            case Vertex::Attribute::Usage::BlendIndices:
                                name = "blendIndices0";
                                break;
                            case Vertex::Attribute::Usage::BlendWeight:
                                name = "blendWeight0";
                                break;
                            case Vertex::Attribute::Usage::Color:
                                name = "color0";
                                break;
                            case Vertex::Attribute::Usage::Normal:
                                name = "normal0";
                                break;
                            case Vertex::Attribute::Usage::Position:
                                name = "position0";
                                break;
                            case Vertex::Attribute::Usage::PositionTransformed:
                                name = "positionT0";
                                break;
                            case Vertex::Attribute::Usage::PointSize:
                                name = "pointSize0";
                                break;
                            case Vertex::Attribute::Usage::Tangent:
                                name = "tangent0";
                                break;
                            case Vertex::Attribute::Usage::TextureCoordinates0:
                                name = "texCoord0";
                                break;
                            case Vertex::Attribute::Usage::TextureCoordinates1:
                                name = "texCoord1";
                                break;
                            default:
                                throw std::runtime_error("Invalid vertex attribute usage");
                        }

                        renderDevice.glBindAttribLocationProc(programId, index, name);
                        ++index;
                    }
                }

                renderDevice.glLinkProgramProc(programId);

                renderDevice.glGetProgramivProc(programId, GL_LINK_STATUS, &status);
                if (status == GL_FALSE)
                    throw std::runtime_error("Failed to link shader" + getProgramMessage());

                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to get shader link status");

                renderDevice.glDetachShaderProc(programId, vertexShaderId);
                renderDevice.glDeleteShaderProc(vertexShaderId);
                vertexShaderId = 0;

                renderDevice.glDetachShaderProc(programId, fragmentShaderId);
                renderDevice.glDeleteShaderProc(fragmentShaderId);
                fragmentShaderId = 0;

                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to detach shader");

                renderDevice.useProgram(programId);

                const GLint texture0Location = renderDevice.glGetUniformLocationProc(programId, "texture0");
                if (texture0Location != -1) renderDevice.glUniform1iProc(texture0Location, 0);

                const GLint texture1Location = renderDevice.glGetUniformLocationProc(programId, "texture1");
                if (texture1Location != -1) renderDevice.glUniform1iProc(texture1Location, 1);

                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to get uniform location");

                if (!fragmentShaderConstantInfo.empty())
                {
                    fragmentShaderConstantLocations.clear();
                    fragmentShaderConstantLocations.reserve(fragmentShaderConstantInfo.size());

                    for (const auto& info : fragmentShaderConstantInfo)
                    {
                        const GLint location = renderDevice.glGetUniformLocationProc(programId, info.first.c_str());

                        if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                            throw std::system_error(makeErrorCode(error), "Failed to get OpenGL uniform location");

                        if (location == -1)
                            throw std::runtime_error("Failed to get OpenGL uniform location");

                        fragmentShaderConstantLocations.emplace_back(location, info.second);
                    }
                }

                if (!vertexShaderConstantInfo.empty())
                {
                    vertexShaderConstantLocations.clear();
                    vertexShaderConstantLocations.reserve(vertexShaderConstantInfo.size());

                    for (const auto& info : vertexShaderConstantInfo)
                    {
                        const GLint location = renderDevice.glGetUniformLocationProc(programId, info.first.c_str());

                        if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                            throw std::system_error(makeErrorCode(error), "Failed to get OpenGL uniform location");

                        if (location == -1)
                            throw std::runtime_error("Failed to get OpenGL uniform location");

                        vertexShaderConstantLocations.emplace_back(location, info.second);
                    }
                }
            }
        } // namespace opengl
    } // namespace graphics
} // namespace ouzel

#endif
