// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_NODE_HPP
#define OUZEL_AUDIO_NODE_HPP

#include <cstdint>
#include <vector>
#include "math/Quaternion.hpp"
#include "math/Vector3.hpp"

namespace ouzel
{
    namespace audio
    {
        class Object
        {
        public:
            Object() {}
            virtual ~Object() {}

            Object(const Object&) = delete;
            Object& operator=(const Object&) = delete;

            Object(Object&&) = delete;
            Object& operator=(Object&&) = delete;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_NODE_HPP