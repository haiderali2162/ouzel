// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <memory>
#include <string>
#include <map>
#include "audio/SoundData.hpp"
#include "files/FileSystem.hpp"
#include "graphics/BlendState.hpp"
#include "graphics/Material.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Texture.hpp"
#include "gui/Font.hpp"
#include "scene/MeshData.hpp"
#include "scene/SpriteData.hpp"
#include "scene/ParticleSystemData.hpp"

namespace ouzel
{
    namespace assets
    {
        class Cache;

        class Bundle final
        {
            friend Cache;
        public:
            explicit Bundle(Cache& initCache);
            ~Bundle();

            Bundle(const Bundle&) = delete;
            Bundle& operator=(const Bundle&) = delete;

            Bundle(Bundle&&) = delete;
            Bundle& operator=(Bundle&&) = delete;

            void loadAsset(uint32_t loaderType, const std::string& filename, bool mipmaps = true);
            void loadAssets(const std::string& filename);

            void clear();

            std::shared_ptr<graphics::Texture> getTexture(const std::string& filename) const;
            void setTexture(const std::string& filename, const std::shared_ptr<graphics::Texture>& texture);
            void releaseTextures();

            std::shared_ptr<graphics::Shader> getShader(const std::string& shaderName) const;
            void setShader(const std::string& shaderName, const std::shared_ptr<graphics::Shader>& shader);
            void releaseShaders();

            std::shared_ptr<graphics::BlendState> getBlendState(const std::string& blendStateName) const;
            void setBlendState(const std::string& blendStateName, const std::shared_ptr<graphics::BlendState>& blendState);
            void releaseBlendStates();

            void preloadSpriteData(const std::string& filename, bool mipmaps = true,
                                   uint32_t spritesX = 1, uint32_t spritesY = 1,
                                   const Vector2& pivot = Vector2(0.5F, 0.5F));
            const scene::SpriteData* getSpriteData(const std::string& filename) const;
            void setSpriteData(const std::string& filename, const scene::SpriteData& newSpriteData);
            void releaseSpriteData();

            const scene::ParticleSystemData* getParticleSystemData(const std::string& filename) const;
            void setParticleSystemData(const std::string& filename, const scene::ParticleSystemData& newParticleSystemData);
            void releaseParticleSystemData();

            std::shared_ptr<Font> getFont(const std::string& filename) const;
            void setFont(const std::string& filename, const std::shared_ptr<Font>& font);
            void releaseFonts();

            std::shared_ptr<audio::SoundData> getSoundData(const std::string& filename) const;
            void setSoundData(const std::string& filename, const std::shared_ptr<audio::SoundData>& newSoundData);
            void releaseSoundData();

            std::shared_ptr<graphics::Material> getMaterial(const std::string& filename) const;
            void setMaterial(const std::string& filename, const std::shared_ptr<graphics::Material>& material);
            void releaseMaterials();

            const scene::MeshData* getMeshData(const std::string& filename) const;
            void setMeshData(const std::string& filename, const scene::MeshData& newMeshData);
            void releaseMeshData();

        protected:
            Cache& cache;

            std::map<std::string, std::shared_ptr<graphics::Texture>> textures;
            std::map<std::string, std::shared_ptr<graphics::Shader>> shaders;
            std::map<std::string, scene::ParticleSystemData> particleSystemData;
            std::map<std::string, std::shared_ptr<graphics::BlendState>> blendStates;
            std::map<std::string, scene::SpriteData> spriteData;
            std::map<std::string, std::shared_ptr<Font>> fonts;
            std::map<std::string, std::shared_ptr<audio::SoundData>> soundData;
            std::map<std::string, std::shared_ptr<graphics::Material>> materials;
            std::map<std::string, scene::MeshData> meshData;
        };
    } // namespace assets
} // namespace ouzel