// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/MouseDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class MouseDeviceEm: public MouseDevice
        {
        public:
            MouseDeviceEm(InputSystem& initInputSystem,
                          uint32_t initId):
                MouseDevice(initInputSystem, initId)
            {
            }

            virtual ~MouseDeviceEm() {}
        };
    } // namespace input
} // namespace ouzel