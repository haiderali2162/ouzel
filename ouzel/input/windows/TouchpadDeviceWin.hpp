// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/TouchpadDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class TouchpadDeviceWin: public TouchpadDevice
        {
        public:
            TouchpadDeviceWin(InputSystem& initInputSystem,
                              uint32_t initId):
                TouchpadDevice(initInputSystem, initId)
            {
            }

            virtual ~TouchpadDeviceWin() {}
        };
    } // namespace input
} // namespace ouzel