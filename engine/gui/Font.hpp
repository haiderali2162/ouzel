// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GUI_FONT_HPP
#define OUZEL_GUI_FONT_HPP

#include <cstdint>
#include <string>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>
#include "math/Color.hpp"
#include "graphics/Texture.hpp"
#include "graphics/Vertex.hpp"

namespace ouzel
{
    namespace gui
    {
        class Font
        {
        public:
            Font() = default;
            virtual ~Font() = default;

            Font(const Font&) = delete;
            Font& operator=(const Font&) = delete;

            Font(Font&&) = delete;
            Font& operator=(Font&&) = delete;

            // TODO: return data in a struct

            using RenderData = std::tuple<std::vector<std::uint16_t>,
                std::vector<graphics::Vertex>,
                std::shared_ptr<graphics::Texture>>;

            virtual RenderData getRenderData(const std::string& text,
                                             Color color,
                                             float fontSize,
                                             const Vector2F& anchor) const = 0;
        };
    } // namespace gui
} // namespace ouzel

#endif // OUZEL_GUI_FONT_HPP
