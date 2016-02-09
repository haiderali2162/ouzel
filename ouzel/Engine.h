// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <set>
#include "Types.h"
#include "Noncopyable.h"
#include "Renderer.h"
#include "SceneManager.h"
#include "FileSystem.h"
#include "Input.h"
#include "EventDispatcher.h"
#include "App.h"

namespace ouzel
{
    class UpdateCallback
    {
    public:
        std::function<void(float)> callback;
    };
    
    class Engine: public Noncopyable
    {
    public:
        static EnginePtr getInstance();
        
        virtual ~Engine();
        
        void setArgs(const std::vector<std::string>& args);
        const std::vector<std::string>& getArgs() { return _args; }
        
        std::set<Renderer::Driver> getAvailableDrivers() const;
        
        bool init();

        const EventDispatcherPtr& getEventDispatcher() const { return _eventDispatcher; }
        const CachePtr& getCache() const { return _cache; }
        const RendererPtr& getRenderer() const { return _renderer; }
        const SceneManagerPtr& getSceneManager() const { return _sceneManager; }
        const FileSystemPtr& getFileSystem() const { return _fileSystem; }
        const InputPtr& getInput() const { return _input; }
        
        void begin();
        void end();
        void run();
        
        float getTargetFPS() const { return _targetFPS; }
        float getFPS() const { return _currentFPS; }
        
        void scheduleUpdate(const UpdateCallbackPtr& callback);
        void unscheduleUpdate(const UpdateCallbackPtr& callback);
        
        void lock();
        void unlock();
        
    protected:
        Engine();
        
        std::vector<std::string> _args;
        
        AppPtr _app;
        
        EventDispatcherPtr _eventDispatcher;
        CachePtr _cache;
        RendererPtr _renderer;
        SceneManagerPtr _sceneManager;
        FileSystemPtr _fileSystem;
        InputPtr _input;
        
        float _targetFPS;
        float _currentFPS = 0.0f;
        uint64_t _previousFrameTime;
        
        std::vector<UpdateCallbackPtr> _updateCallbacks;
        std::set<UpdateCallbackPtr> _updateCallbackAddList;
        std::set<UpdateCallbackPtr> _updateCallbackRemoveList;
        
        int32_t _locked = 0;
    };
}
