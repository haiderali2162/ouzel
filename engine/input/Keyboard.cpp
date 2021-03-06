// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include <iterator>
#include "Keyboard.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"

namespace ouzel
{
    namespace input
    {
        Keyboard::Keyboard(InputManager& initInputManager, DeviceId initDeviceId):
            Controller(initInputManager, Controller::Type::Keyboard, initDeviceId)
        {
        }

        bool Keyboard::handleKeyPress(Keyboard::Key key)
        {
            auto event = std::make_unique<KeyboardEvent>();
            event->keyboard = this;
            event->key = key;

            if (!keyStates[static_cast<std::uint32_t>(key)])
            {
                keyStates[static_cast<std::uint32_t>(key)] = true;

                event->type = Event::Type::KeyboardKeyPress;
                return engine->getEventDispatcher().dispatchEvent(std::move(event));
            }

            return false;
        }

        bool Keyboard::handleKeyRelease(Keyboard::Key key)
        {
            keyStates[static_cast<std::uint32_t>(key)] = false;

            auto event = std::make_unique<KeyboardEvent>();
            event->type = Event::Type::KeyboardKeyRelease;
            event->keyboard = this;
            event->key = key;

            return engine->getEventDispatcher().dispatchEvent(std::move(event));
        }
    } // namespace input
} // namespace ouzel
