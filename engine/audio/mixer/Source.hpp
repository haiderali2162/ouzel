// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_SOURCE_HPP
#define OUZEL_AUDIO_MIXER_SOURCE_HPP

#include <cstdint>
#include <vector>

namespace ouzel
{
    namespace audio
    {
        namespace mixer
        {
            class Source
            {
            public:
                Source() noexcept = default;
                virtual ~Source() = default;

                virtual void play() = 0;
                virtual void stop(bool shouldReset) = 0;

                virtual void getSamples(std::uint32_t frames, std::uint32_t channels, std::uint32_t sampleRate, std::vector<float>& samples) = 0;
            };
        }
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_SOURCE_HPP
