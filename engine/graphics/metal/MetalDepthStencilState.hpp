// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_METALDEPTHSTENCILSTATE_HPP
#define OUZEL_GRAPHICS_METALDEPTHSTENCILSTATE_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#if defined(__OBJC__)
#  import <CoreVideo/CoreVideo.h>
#  import <QuartzCore/QuartzCore.h>
#  import <Metal/Metal.h>
typedef id<MTLDepthStencilState> MTLDepthStencilStatePtr;
#else
#  include <objc/objc.h>
typedef id MTLDepthStencilStatePtr;
#endif

#include "graphics/metal/MetalRenderResource.hpp"
#include "graphics/metal/MetalPointer.hpp"
#include "graphics/CompareFunction.hpp"
#include "graphics/StencilOperation.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace metal
        {
            class RenderDevice;

            class DepthStencilState final: public RenderResource
            {
            public:
                DepthStencilState(RenderDevice& initRenderDevice,
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
                                  CompareFunction initBackFaceStencilCompareFunction);

                inline auto& getDepthStencilState() const noexcept { return depthStencilState; }

            private:
                Pointer<MTLDepthStencilStatePtr> depthStencilState;
            };
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_METALDEPTHSTENCILSTATE_HPP
