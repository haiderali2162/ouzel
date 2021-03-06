// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_PROCESSOR_HPP
#define OUZEL_AUDIO_MIXER_PROCESSOR_HPP

#include "audio/mixer/Object.hpp"
#include "audio/mixer/Bus.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace mixer
        {
            class Processor: public Object
            {
                friend Bus;
            public:
                Processor() noexcept = default;
                ~Processor() override
                {
                    if (bus) bus->removeProcessor(this);
                }

                Processor(const Processor&) = delete;
                Processor& operator=(const Processor&) = delete;

                Processor(Processor&&) = delete;
                Processor& operator=(Processor&&) = delete;

                virtual void process(std::uint32_t frames, std::uint32_t channels, std::uint32_t sampleRate,
                                     std::vector<float>& samples) = 0;

                inline auto isEnabled() const noexcept { return enabled; }
                inline void setEnabled(bool newEnabled) { enabled = newEnabled; }

            private:
                Bus* bus = nullptr;
                bool enabled = true;
            };
        }
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_PROCESSOR_HPP
