// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include "graphics/DataType.hpp"
#include "math/Vector2.hpp"
#include "math/Vector3.hpp"
#include "math/Color.hpp"

namespace ouzel
{
    namespace graphics
    {
        class Vertex
        {
        public:
            struct Attribute
            {
                enum class Usage
                {
                    NONE,
                    BINORMAL,
                    BLEND_INDICES,
                    BLEND_WEIGHT,
                    COLOR,
                    NORMAL,
                    POSITION,
                    POSITION_TRANSFORMED,
                    POINT_SIZE,
                    TANGENT,
                    TEXTURE_COORDINATES0,
                    TEXTURE_COORDINATES1
                };

                Attribute(Usage aUsage, DataType aDataType, bool aNormalized):
                    usage(aUsage), dataType(aDataType), normalized(aNormalized) {}
                Usage usage = Usage::NONE;
                DataType dataType = DataType::NONE;
                bool normalized = false;
            };
            
            static const uint32_t VERTEX_ATTRIBUTE_COUNT = 5;

            static const std::vector<Attribute> ATTRIBUTES;

            Vertex();
            Vertex(const Vector3& aPosition, Color aColor, const Vector2& aTexCoord, const Vector3& aNormal);

            Vector3 position;
            Color color;
            Vector2 texCoords[2];
            Vector3 normal;
        };
    } // namespace graphics
} // namespace ouzel
