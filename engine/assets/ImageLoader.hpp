// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_ASSETS_IMAGELOADER_HPP
#define OUZEL_ASSETS_IMAGELOADER_HPP

#include "assets/Loader.hpp"

namespace ouzel
{
    namespace assets
    {
        class ImageLoader final: public Loader
        {
        public:
            explicit ImageLoader(Cache& initCache);
            bool loadAsset(Bundle& bundle,
                           const std::string& name,
                           const std::vector<std::uint8_t>& data,
                           bool mipmaps = true) final;
        };
    } // namespace assets
} // namespace ouzel

#endif // OUZEL_ASSETS_IMAGELOADER_HPP
