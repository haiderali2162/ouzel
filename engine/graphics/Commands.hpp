// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_COMMANDS_HPP
#define OUZEL_GRAPHICS_COMMANDS_HPP

#include <queue>
#include <string>
#include "graphics/BlendFactor.hpp"
#include "graphics/BlendOperation.hpp"
#include "graphics/BufferType.hpp"
#include "graphics/ColorMask.hpp"
#include "graphics/CompareFunction.hpp"
#include "graphics/CubeFace.hpp"
#include "graphics/DataType.hpp"
#include "graphics/DrawMode.hpp"
#include "graphics/Flags.hpp"
#include "graphics/PixelFormat.hpp"
#include "graphics/RasterizerState.hpp"
#include "graphics/SamplerFilter.hpp"
#include "graphics/SamplerAddressMode.hpp"
#include "graphics/StencilOperation.hpp"
#include "graphics/TextureType.hpp"
#include "graphics/Vertex.hpp"
#include "math/Color.hpp"
#include "math/Rect.hpp"

namespace ouzel
{
    namespace graphics
    {
        class Command
        {
        public:
            enum class Type
            {
                Stop,
                Resize,
                Present,
                DeleteResource,
                InitRenderTarget,
                SetRenderTarget,
                ClearRenderTarget,
                Blit,
                Compute,
                SetScissorTest,
                SetViewport,
                InitDepthStencilState,
                SetDepthStencilState,
                SetPipelineState,
                Draw,
                PushDebugMarker,
                PopDebugMarker,
                InitBlendState,
                InitBuffer,
                SetBufferData,
                InitShader,
                SetShaderConstants,
                InitTexture,
                SetTextureData,
                SetTextureParameters,
                SetTextures
            };

            explicit constexpr Command(Type initType) noexcept:
                type(initType)
            {
            }

            virtual ~Command() = default;

            const Type type;
        };

        class StopCommand final: public Command
        {
        public:
            constexpr StopCommand() noexcept:
                Command(Command::Type::Stop)
            {}
        };

        class ResizeCommand final: public Command
        {
        public:
            explicit constexpr ResizeCommand(const Size2U& initSize) noexcept:
                Command(Command::Type::Resize),
                size(initSize)
            {}

            const Size2U size;
        };

        class PresentCommand final: public Command
        {
        public:
            constexpr PresentCommand() noexcept:
                Command(Command::Type::Present)
            {}
        };

        class DeleteResourceCommand final: public Command
        {
        public:
            explicit constexpr DeleteResourceCommand(std::uintptr_t initResource) noexcept:
                Command(Command::Type::DeleteResource),
                resource(initResource)
            {}

            const std::uintptr_t resource;
        };

        class InitRenderTargetCommand final: public Command
        {
        public:
            InitRenderTargetCommand(std::uintptr_t initRenderTarget,
                                    const std::set<std::uintptr_t>& initColorTextures,
                                    std::uintptr_t initDepthTexture) noexcept:
                Command(Command::Type::InitRenderTarget),
                renderTarget(initRenderTarget),
                colorTextures(initColorTextures),
                depthTexture(initDepthTexture)
            {
            }

            const std::uintptr_t renderTarget;
            const std::set<std::uintptr_t> colorTextures;
            const std::uintptr_t depthTexture;
        };

        class SetRenderTargetCommand final: public Command
        {
        public:
            explicit constexpr SetRenderTargetCommand(std::uintptr_t initRenderTarget) noexcept:
                Command(Command::Type::SetRenderTarget),
                renderTarget(initRenderTarget)
            {
            }

            const std::uintptr_t renderTarget;
        };

        class ClearRenderTargetCommand final: public Command
        {
        public:
            constexpr ClearRenderTargetCommand(bool initClearColorBuffer,
                                               bool initClearDepthBuffer,
                                               bool initClearStencilBuffer,
                                               Color initClearColor,
                                               float initClearDepth,
                                               std::uint32_t initClearStencil) noexcept:
                Command(Command::Type::ClearRenderTarget),
                clearColorBuffer(initClearColorBuffer),
                clearDepthBuffer(initClearDepthBuffer),
                clearStencilBuffer(initClearStencilBuffer),
                clearColor(initClearColor),
                clearDepth(initClearDepth),
                clearStencil(initClearStencil)
            {
            }

            const bool clearColorBuffer;
            const bool clearDepthBuffer;
            const bool clearStencilBuffer;
            const Color clearColor;
            const float clearDepth;
            const std::uint32_t clearStencil;
        };

        // TODO: implement
        class BlitCommand final: public Command
        {
        public:
            constexpr BlitCommand(std::uintptr_t initSourceTexture,
                                  std::uint32_t initSourceLevel,
                                  std::uint32_t initSourceX,
                                  std::uint32_t initSourceY,
                                  std::uint32_t initSourceWidth,
                                  std::uint32_t initSourceHeight,
                                  std::uintptr_t initDestinationTexture,
                                  std::uint32_t initDestinationLevel,
                                  std::uint32_t initDestinationX,
                                  std::uint32_t initDestinationY) noexcept:
                Command(Command::Type::Blit),
                sourceTexture(initSourceTexture),
                sourceLevel(initSourceLevel),
                sourceX(initSourceX),
                sourceY(initSourceY),
                sourceWidth(initSourceWidth),
                sourceHeight(initSourceHeight),
                destinationTexture(initDestinationTexture),
                destinationLevel(initDestinationLevel),
                destinationX(initDestinationX),
                destinationY(initDestinationY)
            {
            }

            const std::uintptr_t sourceTexture;
            const std::uint32_t sourceLevel;
            const std::uint32_t sourceX;
            const std::uint32_t sourceY;
            const std::uint32_t sourceWidth;
            const std::uint32_t sourceHeight;
            const std::uintptr_t destinationTexture;
            const std::uint32_t destinationLevel;
            const std::uint32_t destinationX;
            const std::uint32_t destinationY;
        };

        // TODO: implement
        class ComputeCommand final: public Command
        {
        public:
            explicit constexpr ComputeCommand(std::uintptr_t initShader) noexcept:
                Command(Command::Type::Compute),
                shader(initShader)
            {
            }

            const std::uintptr_t shader;
        };

        class SetScissorTestCommand final: public Command
        {
        public:
            constexpr SetScissorTestCommand(bool initEnabled,
                                            const RectF& initRectangle) noexcept:
                Command(Command::Type::SetScissorTest),
                enabled(initEnabled),
                rectangle(initRectangle)
            {
            }

            const bool enabled;
            const RectF rectangle;
        };

        class SetViewportCommand final: public Command
        {
        public:
            explicit constexpr SetViewportCommand(const RectF& initViewport) noexcept:
                Command(Command::Type::SetViewport),
                viewport(initViewport)
            {
            }

            const RectF viewport;
        };

        class InitDepthStencilStateCommand final: public Command
        {
        public:
            constexpr InitDepthStencilStateCommand(std::uintptr_t initDepthStencilState,
                                                   bool initDepthTest,
                                                   bool initDepthWrite,
                                                   CompareFunction initCompareFunction,
                                                   bool initStencilEnabled,
                                                   std::uint32_t initStencilReadMask,
                                                   std::uint32_t initStencilWriteMask,
                                                   StencilOperation initFrontFaceStencilFailureOperation,
                                                   StencilOperation initFrontFaceStencilDepthFailureOperation,
                                                   StencilOperation initFrontFaceStencilPassOperation,
                                                   CompareFunction initFrontFaceStencilCompareFunction,
                                                   StencilOperation initBackFaceStencilFailureOperation,
                                                   StencilOperation initBackFaceStencilDepthFailureOperation,
                                                   StencilOperation initBackFaceStencilPassOperation,
                                                   CompareFunction initBackFaceStencilCompareFunction) noexcept:
                Command(Command::Type::InitDepthStencilState),
                depthStencilState(initDepthStencilState),
                depthTest(initDepthTest),
                depthWrite(initDepthWrite),
                compareFunction(initCompareFunction),
                stencilEnabled(initStencilEnabled),
                stencilReadMask(initStencilReadMask),
                stencilWriteMask(initStencilWriteMask),
                frontFaceStencilFailureOperation(initFrontFaceStencilFailureOperation),
                frontFaceStencilDepthFailureOperation(initFrontFaceStencilDepthFailureOperation),
                frontFaceStencilPassOperation(initFrontFaceStencilPassOperation),
                frontFaceStencilCompareFunction(initFrontFaceStencilCompareFunction),
                backFaceStencilFailureOperation(initBackFaceStencilFailureOperation),
                backFaceStencilDepthFailureOperation(initBackFaceStencilDepthFailureOperation),
                backFaceStencilPassOperation(initBackFaceStencilPassOperation),
                backFaceStencilCompareFunction(initBackFaceStencilCompareFunction)
            {
            }

            const std::uintptr_t depthStencilState;
            const bool depthTest;
            const bool depthWrite;
            const CompareFunction compareFunction;
            const bool stencilEnabled;
            const std::uint32_t stencilReadMask;
            const std::uint32_t stencilWriteMask;
            const StencilOperation frontFaceStencilFailureOperation;
            const StencilOperation frontFaceStencilDepthFailureOperation;
            const StencilOperation frontFaceStencilPassOperation;
            const CompareFunction frontFaceStencilCompareFunction;
            const StencilOperation backFaceStencilFailureOperation;
            const StencilOperation backFaceStencilDepthFailureOperation;
            const StencilOperation backFaceStencilPassOperation;
            const CompareFunction backFaceStencilCompareFunction;
        };

        class SetDepthStencilStateCommand final: public Command
        {
        public:
            constexpr SetDepthStencilStateCommand(std::uintptr_t initDepthStencilState,
                                                  std::uint32_t initStencilReferenceValue) noexcept:
                Command(Command::Type::SetDepthStencilState),
                depthStencilState(initDepthStencilState),
                stencilReferenceValue(initStencilReferenceValue)
            {
            }

            const std::uintptr_t depthStencilState;
            const std::uint32_t stencilReferenceValue;
        };

        class SetPipelineStateCommand final: public Command
        {
        public:
            constexpr SetPipelineStateCommand(std::uintptr_t initBlendState,
                                              std::uintptr_t initShader,
                                              CullMode initCullMode,
                                              FillMode initFillMode) noexcept:
                Command(Command::Type::SetPipelineState),
                blendState(initBlendState),
                shader(initShader),
                cullMode(initCullMode),
                fillMode(initFillMode)
            {
            }

            const std::uintptr_t blendState;
            const std::uintptr_t shader;
            const CullMode cullMode;
            const FillMode fillMode;
        };

        class DrawCommand final: public Command
        {
        public:
            constexpr DrawCommand(std::uintptr_t initIndexBuffer,
                                  std::uint32_t initIndexCount,
                                  std::uint32_t initIndexSize,
                                  std::uintptr_t initVertexBuffer,
                                  DrawMode initDrawMode,
                                  std::uint32_t initStartIndex) noexcept:
                Command(Command::Type::Draw),
                indexBuffer(initIndexBuffer),
                indexCount(initIndexCount),
                indexSize(initIndexSize),
                vertexBuffer(initVertexBuffer),
                drawMode(initDrawMode),
                startIndex(initStartIndex)
            {
            }

            const std::uintptr_t indexBuffer;
            const std::uint32_t indexCount;
            const std::uint32_t indexSize;
            const std::uintptr_t vertexBuffer;
            const DrawMode drawMode;
            const std::uint32_t startIndex;
        };

        class PushDebugMarkerCommand final: public Command
        {
        public:
            explicit PushDebugMarkerCommand(const std::string& initName) noexcept:
                Command(Command::Type::PushDebugMarker),
                name(initName)
            {
            }

            const std::string name;
        };

        class PopDebugMarkerCommand final: public Command
        {
        public:
            constexpr PopDebugMarkerCommand() noexcept:
                Command(Command::Type::PopDebugMarker)
            {
            }
        };

        class InitBlendStateCommand final: public Command
        {
        public:
            constexpr InitBlendStateCommand(std::uintptr_t initBlendState,
                                            bool initEnableBlending,
                                            BlendFactor initColorBlendSource,
                                            BlendFactor initColorBlendDest,
                                            BlendOperation initColorOperation,
                                            BlendFactor initAlphaBlendSource,
                                            BlendFactor initAlphaBlendDest,
                                            BlendOperation initAlphaOperation,
                                            ColorMask initColorMask) noexcept:
                Command(Command::Type::InitBlendState),
                blendState(initBlendState),
                enableBlending(initEnableBlending),
                colorBlendSource(initColorBlendSource),
                colorBlendDest(initColorBlendDest),
                colorOperation(initColorOperation),
                alphaBlendSource(initAlphaBlendSource),
                alphaBlendDest(initAlphaBlendDest),
                alphaOperation(initAlphaOperation),
                colorMask(initColorMask)
            {
            }

            const std::uintptr_t blendState;
            const bool enableBlending;
            const BlendFactor colorBlendSource;
            const BlendFactor colorBlendDest;
            const BlendOperation colorOperation;
            const BlendFactor alphaBlendSource;
            const BlendFactor alphaBlendDest;
            const BlendOperation alphaOperation;
            const ColorMask colorMask;
        };

        class InitBufferCommand final: public Command
        {
        public:
            InitBufferCommand(std::uintptr_t initBuffer,
                              BufferType initBufferType,
                              std::uint32_t initFlags,
                              const std::vector<std::uint8_t>& initData,
                              std::uint32_t initSize) noexcept:
                Command(Command::Type::InitBuffer),
                buffer(initBuffer),
                bufferType(initBufferType),
                flags(initFlags),
                data(initData),
                size(initSize)
            {
            }

            const std::uintptr_t buffer;
            const BufferType bufferType;
            const std::uint32_t flags;
            const std::vector<std::uint8_t> data;
            const std::uint32_t size;
        };

        class SetBufferDataCommand final: public Command
        {
        public:
            SetBufferDataCommand(std::uintptr_t initBuffer,
                                 const std::vector<std::uint8_t>& initData) noexcept:
                Command(Command::Type::SetBufferData),
                buffer(initBuffer),
                data(initData)
            {
            }

            const std::uintptr_t buffer;
            const std::vector<std::uint8_t> data;
        };

        class InitShaderCommand final: public Command
        {
        public:
            InitShaderCommand(std::uintptr_t initShader,
                              const std::vector<std::uint8_t>& initFragmentShader,
                              const std::vector<std::uint8_t>& initVertexShader,
                              const std::set<Vertex::Attribute::Usage>& initVertexAttributes,
                              const std::vector<std::pair<std::string, DataType>>& initFragmentShaderConstantInfo,
                              const std::vector<std::pair<std::string, DataType>>& initVertexShaderConstantInfo,
                              const std::string& initFragmentShaderFunction,
                              const std::string& initVertexShaderFunction) noexcept:
                Command(Command::Type::InitShader),
                shader(initShader),
                fragmentShader(initFragmentShader),
                vertexShader(initVertexShader),
                vertexAttributes(initVertexAttributes),
                fragmentShaderConstantInfo(initFragmentShaderConstantInfo),
                vertexShaderConstantInfo(initVertexShaderConstantInfo),
                fragmentShaderFunction(initFragmentShaderFunction),
                vertexShaderFunction(initVertexShaderFunction)
            {
            }

            const std::uintptr_t shader;
            const std::vector<std::uint8_t> fragmentShader;
            const std::vector<std::uint8_t> vertexShader;
            const std::set<Vertex::Attribute::Usage> vertexAttributes;
            const std::vector<std::pair<std::string, DataType>> fragmentShaderConstantInfo;
            const std::vector<std::pair<std::string, DataType>> vertexShaderConstantInfo;
            const std::string fragmentShaderFunction;
            const std::string vertexShaderFunction;
        };

        class SetShaderConstantsCommand final: public Command
        {
        public:
            SetShaderConstantsCommand(std::vector<std::vector<float>> initFragmentShaderConstants,
                                      std::vector<std::vector<float>> initVertexShaderConstants) noexcept:
                Command(Command::Type::SetShaderConstants),
                fragmentShaderConstants(initFragmentShaderConstants),
                vertexShaderConstants(initVertexShaderConstants)
            {
            }

            const std::vector<std::vector<float>> fragmentShaderConstants;
            const std::vector<std::vector<float>> vertexShaderConstants;
        };

        class InitTextureCommand final: public Command
        {
        public:
            InitTextureCommand(std::uintptr_t initTexture,
                               const std::vector<std::pair<Size2U, std::vector<std::uint8_t>>>& initLevels,
                               TextureType initTextureType,
                               std::uint32_t initFlags,
                               std::uint32_t initSampleCount,
                               PixelFormat initPixelFormat) noexcept:
                Command(Command::Type::InitTexture),
                texture(initTexture),
                levels(initLevels),
                textureType(initTextureType),
                flags(initFlags),
                sampleCount(initSampleCount),
                pixelFormat(initPixelFormat)
            {
            }

            const std::uintptr_t texture;
            const std::vector<std::pair<Size2U, std::vector<std::uint8_t>>> levels;
            const TextureType textureType;
            const std::uint32_t flags;
            const std::uint32_t sampleCount;
            const PixelFormat pixelFormat;
        };

        class SetTextureDataCommand final: public Command
        {
        public:
            SetTextureDataCommand(std::uintptr_t initTexture,
                                  const std::vector<std::pair<Size2U, std::vector<std::uint8_t>>>& initLevels) noexcept:
                Command(Command::Type::SetTextureData),
                texture(initTexture),
                levels(initLevels),
                face(CubeFace::PositiveX)
            {
            }

            SetTextureDataCommand(std::uintptr_t initTexture,
                                  const std::vector<std::pair<Size2U, std::vector<std::uint8_t>>>& initLevels,
                                  CubeFace initFace) noexcept:
                Command(Command::Type::SetTextureData),
                texture(initTexture),
                levels(initLevels),
                face(initFace)
            {
            }

            const std::uintptr_t texture;
            const std::vector<std::pair<Size2U, std::vector<std::uint8_t>>> levels;
            const CubeFace face;
        };

        class SetTextureParametersCommand final: public Command
        {
        public:
            constexpr SetTextureParametersCommand(std::uintptr_t initTexture,
                                                  SamplerFilter initFilter,
                                                  SamplerAddressMode initAddressX,
                                                  SamplerAddressMode initAddressY,
                                                  SamplerAddressMode initAddressZ,
                                                  Color initBorderColor,
                                                  std::uint32_t initMaxAnisotropy) noexcept:
                Command(Command::Type::SetTextureParameters),
                texture(initTexture),
                filter(initFilter),
                addressX(initAddressX),
                addressY(initAddressY),
                addressZ(initAddressZ),
                borderColor(initBorderColor),
                maxAnisotropy(initMaxAnisotropy)
            {
            }

            const std::uintptr_t texture;
            const SamplerFilter filter;
            const SamplerAddressMode addressX;
            const SamplerAddressMode addressY;
            const SamplerAddressMode addressZ;
            const Color borderColor;
            const std::uint32_t maxAnisotropy;
        };

        class SetTexturesCommand final: public Command
        {
        public:
            explicit SetTexturesCommand(const std::vector<std::uintptr_t>& initTextures) noexcept:
                Command(Command::Type::SetTextures),
                textures(initTextures)
            {
            }

            const std::vector<std::uintptr_t> textures;
        };

        class CommandBuffer final
        {
        public:
            CommandBuffer() = default;
            explicit CommandBuffer(const std::string& initName) noexcept:
                name(initName)
            {
            }

            inline auto& getName() const noexcept { return name; }

            inline auto isEmpty() const noexcept { return commands.empty(); }

            inline void pushCommand(std::unique_ptr<Command> command)
            {
                commands.push(std::move(command));
            }

            inline std::unique_ptr<Command> popCommand()
            {
                auto result = std::move(commands.front());
                commands.pop();
                return result;
            }

            inline auto& getCommands() const
            {
                return commands;
            }

        private:
            std::string name;
            std::queue<std::unique_ptr<Command>> commands;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_COMMANDS_HPP
