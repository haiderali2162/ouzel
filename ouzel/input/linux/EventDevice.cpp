// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include "EventDevice.hpp"
#include "InputSystemLinux.hpp"
#include "input/GamepadConfig.hpp"
#include "input/KeyboardDevice.hpp"
#include "input/GamepadDevice.hpp"
#include "input/MouseDevice.hpp"
#include "input/TouchpadDevice.hpp"
#include "core/Engine.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"

static const float THUMB_DEADZONE = 0.2F;

static const uint32_t BITS_PER_LONG = 8 * sizeof(long);
#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#define BITS_TO_LONGS(nr) DIV_ROUND_UP(nr, BITS_PER_LONG)

static inline bool isBitSet(const unsigned long* array, int bit)
{
    return (array[bit / BITS_PER_LONG] & (1LL << (bit % BITS_PER_LONG))) != 0;
}

namespace ouzel
{
    namespace input
    {
        EventDevice::EventDevice(InputSystemLinux& inputSystem, const std::string& initFilename):
            filename(initFilename)
        {
            fd = open(filename.c_str(), O_RDONLY);

            if (fd == -1)
                throw SystemError("Failed to open device file");

            if (ioctl(fd, EVIOCGRAB, 1) == -1)
                Log(Log::Level::WARN) << "Failed to grab device";

            char deviceName[256];
            if (ioctl(fd, EVIOCGNAME(sizeof(deviceName) - 1), deviceName) == -1)
                Log(Log::Level::WARN) << "Failed to get device name";
            else
            {
                name = deviceName;
                Log(Log::Level::INFO) << "Got device: " << name;
            }

            unsigned long eventBits[BITS_TO_LONGS(EV_CNT)];
            unsigned long absBits[BITS_TO_LONGS(ABS_CNT)];
            unsigned long relBits[BITS_TO_LONGS(REL_CNT)];
            unsigned long keyBits[BITS_TO_LONGS(KEY_CNT)];

            if (ioctl(fd, EVIOCGBIT(0, sizeof(eventBits)), eventBits) == -1 ||
                ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(absBits)), absBits) == -1 ||
                ioctl(fd, EVIOCGBIT(EV_REL, sizeof(relBits)), relBits) == -1 ||
                ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keyBits)), keyBits) == -1)
                throw SystemError("Failed to get device event bits");

            if (isBitSet(eventBits, EV_KEY) && (
                    isBitSet(keyBits, KEY_1) ||
                    isBitSet(keyBits, KEY_2) ||
                    isBitSet(keyBits, KEY_3) ||
                    isBitSet(keyBits, KEY_4) ||
                    isBitSet(keyBits, KEY_5) ||
                    isBitSet(keyBits, KEY_6) ||
                    isBitSet(keyBits, KEY_7) ||
                    isBitSet(keyBits, KEY_8) ||
                    isBitSet(keyBits, KEY_9) ||
                    isBitSet(keyBits, KEY_0)
                ))
                keyboardDevice.reset(new KeyboardDevice(inputSystem, inputSystem.getNextDeviceId()));

            if (isBitSet(eventBits, EV_ABS) && isBitSet(absBits, ABS_X) && isBitSet(absBits, ABS_Y))
            {
                if ((isBitSet(keyBits, BTN_STYLUS) || isBitSet(keyBits, BTN_TOOL_PEN)) || // tablet
                    (isBitSet(keyBits, BTN_TOOL_FINGER) && !isBitSet(keyBits, BTN_TOOL_PEN)) || // touchpad
                    isBitSet(keyBits, BTN_TOUCH)) // touchscreen
                {
                    touchpadDevice.reset(new TouchpadDevice(inputSystem, inputSystem.getNextDeviceId()));

                    input_absinfo info;

                    if (ioctl(fd, EVIOCGABS(ABS_MT_SLOT), &info) == -1)
                        throw SystemError("Failed to get device info");

                    touchSlots.resize(info.maximum + 1);

                    if (ioctl(fd, EVIOCGABS(ABS_MT_POSITION_X), &info) == -1)
                        throw SystemError("Failed to get device info");

                    touchMinX = info.minimum;
                    touchMaxX = info.maximum;
                    touchRangeX = touchMaxX - touchMinX;

                    if (ioctl(fd, EVIOCGABS(ABS_MT_POSITION_Y), &info) == -1)
                        throw SystemError("Failed to get device info");

                    touchMinY = info.minimum;
                    touchMaxY = info.maximum;
                    touchRangeY = touchMaxY - touchMinY;

                    if (ioctl(fd, EVIOCGABS(ABS_MT_PRESSURE), &info) != -1)
                    {
                        touchMinPressure = info.minimum;
                        touchMaxPressure = info.maximum;
                    }
                }
                else if (isBitSet(keyBits, BTN_MOUSE)) // mouse
                    mouseDevice.reset(new MouseDevice(inputSystem, inputSystem.getNextDeviceId()));
            }
            else if (isBitSet(eventBits, EV_REL) && isBitSet(relBits, REL_X) && isBitSet(relBits, REL_Y))
            {
                if (isBitSet(keyBits, BTN_MOUSE))
                    mouseDevice.reset(new MouseDevice(inputSystem, inputSystem.getNextDeviceId()));
            }

            if (isBitSet(keyBits, BTN_JOYSTICK) || // joystick
                isBitSet(keyBits, BTN_GAMEPAD)) // gamepad
            {
                gamepadDevice.reset(new GamepadDevice(inputSystem, inputSystem.getNextDeviceId()));

                struct input_id id;
                if (ioctl(fd, EVIOCGID, &id) == -1)
                    throw SystemError("Failed to get device info");

                GamepadConfig gamepadConfig = getGamepadConfig(id.vendor, id.product);

                for (size_t i = 0; i < 24; ++i)
                {
                    if (gamepadConfig.buttonMap[i] != Gamepad::Button::NONE)
                    {
                        Button button;
                        button.button = gamepadConfig.buttonMap[i];
                        buttons.insert(std::make_pair(BTN_GAMEPAD + i, button));
                    }
                }

                static const std::array<uint32_t, 6> axisUsageMap = {
                    ABS_X,
                    ABS_Y,
                    ABS_Z,
                    ABS_RX,
                    ABS_RY,
                    ABS_RZ
                };

                for (size_t i = 0; i < 6; ++i)
                {
                    if (gamepadConfig.axisMap[i] != Gamepad::Axis::NONE)
                    {
                        uint32_t usage = axisUsageMap[i];

                        Axis axis;
                        axis.axis = gamepadConfig.axisMap[i];

                        input_absinfo info;

                        if (ioctl(fd, EVIOCGABS(usage), &info) == -1)
                            throw SystemError("Failed to get device info");

                        axis.min = info.minimum;
                        axis.max = info.maximum;
                        axis.range = info.maximum - info.minimum;

                        switch (gamepadConfig.axisMap[i])
                        {
                            case Gamepad::Axis::NONE:
                                break;
                            case Gamepad::Axis::LEFT_THUMB_X:
                                axis.negativeButton = Gamepad::Button::LEFT_THUMB_LEFT;
                                axis.positiveButton = Gamepad::Button::LEFT_THUMB_RIGHT;
                                break;
                            case Gamepad::Axis::LEFT_THUMB_Y:
                                axis.negativeButton = Gamepad::Button::LEFT_THUMB_UP;
                                axis.positiveButton = Gamepad::Button::LEFT_THUMB_DOWN;
                                break;
                            case Gamepad::Axis::RIGHT_THUMB_X:
                                axis.negativeButton = Gamepad::Button::RIGHT_THUMB_LEFT;
                                axis.positiveButton = Gamepad::Button::RIGHT_THUMB_RIGHT;
                                break;
                            case Gamepad::Axis::RIGHT_THUMB_Y:
                                axis.negativeButton = Gamepad::Button::RIGHT_THUMB_UP;
                                axis.positiveButton = Gamepad::Button::RIGHT_THUMB_DOWN;
                                break;
                            case Gamepad::Axis::LEFT_TRIGGER:
                                axis.negativeButton = Gamepad::Button::LEFT_TRIGGER;
                                axis.positiveButton = Gamepad::Button::LEFT_TRIGGER;
                                hasLeftTrigger = true;
                                break;
                            case Gamepad::Axis::RIGHT_TRIGGER:
                                axis.negativeButton = Gamepad::Button::RIGHT_TRIGGER;
                                axis.positiveButton = Gamepad::Button::RIGHT_TRIGGER;
                                hasRightTrigger = true;
                                break;
                        }

                        axes.insert(std::make_pair(usage, axis));
                    }
                }
            }
        }

        EventDevice::~EventDevice()
        {
            if (fd != -1)
            {
                if (ioctl(fd, EVIOCGRAB, 0) == -1)
                    Log(Log::Level::WARN) << "Failed to release device";

                close(fd);
            }
        }

        void EventDevice::update()
        {
            input_event events[32];
            ssize_t bytesRead = read(fd, events, sizeof(events));

            if (bytesRead == -1)
                throw SystemError("Failed to read from " + filename); // TODO: disconnect the device

            int count = bytesRead / sizeof(input_event);

            for (int i = 0; i < count; ++i)
            {
                input_event& event = events[i];

                if (keyboardDevice)
                {
                    switch (event.type)
                    {
                        case EV_KEY:
                            if (event.value == 1 || event.value == 2) // press or repeat
                                keyboardDevice->handleKeyPress(InputSystemLinux::convertKeyCode(event.code));
                            else if (event.value == 0) // release
                                keyboardDevice->handleKeyRelease(InputSystemLinux::convertKeyCode(event.code));
                            break;
                    }
                }
                if (mouseDevice)
                {
                    switch (event.type)
                    {
                        case EV_ABS:
                        {
                            switch (event.code)
                            {
                                case ABS_X:
                                    cursorPosition.x = engine->getWindow()->convertWindowToNormalizedLocation(Vector2(static_cast<float>(event.value), 0.0F)).x;
                                    break;
                                case ABS_Y:
                                    cursorPosition.y = engine->getWindow()->convertWindowToNormalizedLocation(Vector2(0.0F, static_cast<float>(event.value))).y;
                                    break;
                            }

                            mouseDevice->handleMove(cursorPosition);
                            break;
                        }
                        case EV_REL:
                        {
                            Vector2 relativePos;

                            switch (event.code)
                            {
                                case REL_X:
                                    relativePos.x = static_cast<float>(event.value);
                                    break;
                                case REL_Y:
                                    relativePos.y = static_cast<float>(event.value);
                                    break;
                                case REL_WHEEL:
                                    mouseDevice->handleScroll(Vector2(0.0F, static_cast<float>(event.value)), cursorPosition);
                                    break;
                                case REL_HWHEEL:
                                    mouseDevice->handleScroll(Vector2(static_cast<float>(event.value), 0.0F), cursorPosition);
                                    break;
                            }

                            mouseDevice->handleRelativeMove(engine->getWindow()->convertWindowToNormalizedLocation(relativePos));
                            break;
                        }
                        case EV_KEY:
                        {
                            Mouse::Button button = Mouse::Button::NONE;

                            switch (event.code)
                            {
                            case BTN_LEFT:
                                button = Mouse::Button::LEFT;
                                break;
                            case BTN_RIGHT:
                                button = Mouse::Button::RIGHT;
                                break;
                            case BTN_MIDDLE:
                                button = Mouse::Button::MIDDLE;
                                break;
                            }

                            if (event.value == 1)
                                mouseDevice->handleButtonPress(button, cursorPosition);
                            else if (event.value == 0)
                                mouseDevice->handleButtonRelease(button, cursorPosition);
                            break;
                        }
                    }
                }
                if (touchpadDevice)
                {
                    switch (event.type)
                    {
                        case EV_ABS:
                        {
                            switch (event.code)
                            {
                                case ABS_MT_SLOT:
                                {
                                    currentTouchSlot = event.value;
                                    break;
                                }
                                case ABS_MT_TRACKING_ID:
                                {
                                    if (event.value >= 0)
                                    {
                                        touchSlots[currentTouchSlot].trackingId = event.value;
                                        touchSlots[currentTouchSlot].action = Slot::Action::BEGIN;
                                    }
                                    else
                                        touchSlots[currentTouchSlot].action = Slot::Action::END;
                                    break;
                                }
                                case ABS_MT_POSITION_X:
                                {
                                    touchSlots[currentTouchSlot].positionX = event.value;
                                    touchSlots[currentTouchSlot].action = Slot::Action::MOVE;
                                    break;
                                }
                                case ABS_MT_POSITION_Y:
                                {
                                    touchSlots[currentTouchSlot].positionY = event.value;
                                    touchSlots[currentTouchSlot].action = Slot::Action::MOVE;
                                    break;
                                }
                                case ABS_MT_PRESSURE:
                                {
                                    touchSlots[currentTouchSlot].pressure = event.value;
                                    break;
                                }
                            }
                            break;
                        }
                        case EV_SYN:
                        {
                            switch (event.code)
                            {
                                case SYN_REPORT:
                                {
                                    for (Slot& slot : touchSlots)
                                    {
                                        if (slot.action != Slot::Action::NONE)
                                        {
                                            Vector2 position(static_cast<float>(slot.positionX - touchMinX) / touchRangeX,
                                                             static_cast<float>(slot.positionY - touchMinY) / touchRangeY);
                                            float pressure = static_cast<float>(slot.pressure - touchMinPressure) / touchMaxPressure;

                                            switch (slot.action)
                                            {
                                                case Slot::Action::NONE:
                                                    break;
                                                case Slot::Action::BEGIN:
                                                    touchpadDevice->handleTouchBegin(static_cast<uint64_t>(slot.trackingId), position, pressure);
                                                    break;
                                                case Slot::Action::END:
                                                    touchpadDevice->handleTouchEnd(static_cast<uint64_t>(slot.trackingId), position, pressure);
                                                    break;
                                                case Slot::Action::MOVE:
                                                    touchpadDevice->handleTouchMove(static_cast<uint64_t>(slot.trackingId), position, pressure);
                                                    break;
                                            }

                                            slot.action = Slot::Action::NONE;
                                        }
                                    }
                                    break;
                                }
                                case SYN_DROPPED:
                                {
                                    struct input_mt_request_layout
                                    {
                                        __u32 code;
                                        __s32 values[1];
                                    };

                                    size_t size = sizeof(__u32) + sizeof(__s32) * touchSlots.size();
                                    std::vector<uint8_t> data(size);

                                    input_mt_request_layout* request = reinterpret_cast<input_mt_request_layout*>(data.data());

                                    request->code = ABS_MT_TRACKING_ID;
                                    if (ioctl(fd, EVIOCGMTSLOTS(size), request) == -1)
                                        throw SystemError("Failed to get device info");

                                    for (size_t i = 0; i < touchSlots.size(); ++i)
                                    {
                                        if (touchSlots[i].trackingId < 0 &&
                                            request->values[i] >= 0)
                                        {
                                            touchSlots[i].trackingId = request->values[i];
                                            touchSlots[i].action = Slot::Action::BEGIN;
                                        }
                                        else if (touchSlots[i].trackingId >= 0 &&
                                                 request->values[i] < 0)
                                        {
                                            touchSlots[i].trackingId = request->values[i];
                                            touchSlots[i].action = Slot::Action::END;
                                        }
                                    }

                                    request->code = ABS_MT_POSITION_X;
                                    if (ioctl(fd, EVIOCGMTSLOTS(size), request) == -1)
                                        throw SystemError("Failed to get device info");

                                    for (size_t i = 0; i < touchSlots.size(); ++i)
                                    {
                                        if (touchSlots[i].trackingId >= 0 &&
                                            touchSlots[i].positionX != request->values[i])
                                        {
                                            touchSlots[i].positionX = request->values[i];
                                            if (touchSlots[i].action == Slot::Action::NONE)
                                                touchSlots[i].action = Slot::Action::MOVE;
                                        }
                                    }

                                    request->code = ABS_MT_POSITION_Y;
                                    if (ioctl(fd, EVIOCGMTSLOTS(size), request) == -1)
                                        throw SystemError("Failed to get device info");

                                    for (size_t i = 0; i < touchSlots.size(); ++i)
                                    {
                                        if (touchSlots[i].trackingId >= 0 &&
                                            touchSlots[i].positionY != request->values[i])
                                        {
                                            touchSlots[i].positionY = request->values[i];
                                            if (touchSlots[i].action == Slot::Action::NONE)
                                                touchSlots[i].action = Slot::Action::MOVE;
                                        }
                                    }

                                    request->code = ABS_MT_PRESSURE;
                                    if (ioctl(fd, EVIOCGABS(size), request) != -1)
                                    {
                                        for (size_t i = 0; i < touchSlots.size(); ++i)
                                        {
                                            if (touchSlots[i].trackingId >= 0 &&
                                                touchSlots[i].pressure != request->values[i])
                                            {
                                                touchSlots[i].pressure = request->values[i];
                                                if (touchSlots[i].action == Slot::Action::NONE)
                                                    touchSlots[i].action = Slot::Action::MOVE;
                                            }
                                        }
                                    }

                                    input_absinfo info;
                                    if (ioctl(fd, EVIOCGABS(ABS_MT_SLOT), &info) == -1)
                                        throw SystemError("Failed to get device info");
                                    currentTouchSlot = info.value;

                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
                if (gamepadDevice)
                {
                    switch(event.type)
                    {
                        case EV_ABS:
                        {
                            if (event.code == ABS_HAT0X)
                            {
                                if (event.value != 0)
                                    gamepadDevice->handleButtonValueChange((event.value > 0) ? Gamepad::Button::DPAD_RIGHT : Gamepad::Button::DPAD_LEFT, true, 1.0F);
                                else if (hat0XValue != 0)
                                    gamepadDevice->handleButtonValueChange((hat0XValue > 0) ? Gamepad::Button::DPAD_RIGHT : Gamepad::Button::DPAD_LEFT, false, 0.0F);

                                hat0XValue = event.value;
                            }
                            else if (event.code == ABS_HAT0Y)
                            {
                                if (event.value != 0)
                                    gamepadDevice->handleButtonValueChange((event.value > 0) ? Gamepad::Button::DPAD_DOWN : Gamepad::Button::DPAD_UP, true, 1.0F);
                                else if (hat0YValue != 0)
                                    gamepadDevice->handleButtonValueChange((hat0YValue > 0) ? Gamepad::Button::DPAD_DOWN : Gamepad::Button::DPAD_UP, false, 0.0F);

                                hat0YValue = event.value;
                            }

                            auto axisIterator = axes.find(event.code);

                            if (axisIterator != axes.end())
                            {
                                Axis& axis = axisIterator->second;
               
                                handleAxisChange(axis.value,
                                                 event.value,
                                                 axis.min, axis.range,
                                                 axis.negativeButton, axis.positiveButton);

                                axis.value = event.value;
                            }
                            break;
                        }
                        case EV_KEY:
                        {
                            auto buttonIterator = buttons.find(event.code);

                            if (buttonIterator != buttons.end())
                            {
                                Button& button = buttonIterator->second;

                                if ((button.button != Gamepad::Button::LEFT_TRIGGER || !hasLeftTrigger) &&
                                    (button.button != Gamepad::Button::RIGHT_TRIGGER || !hasRightTrigger))
                                {
                                    gamepadDevice->handleButtonValueChange(button.button, event.value > 0, (event.value > 0) ? 1.0F : 0.0F);
                                }

                                button.value = event.value;
                            }
                            break;
                        }
                    }
                }
            }
        }

        void EventDevice::handleAxisChange(int32_t oldValue, int32_t newValue,
                                           int32_t min, int32_t range,
                                           Gamepad::Button negativeButton,
                                           Gamepad::Button positiveButton)
        {
            if (negativeButton == positiveButton)
            {
                float floatValue = static_cast<float>(newValue - min) / range;

                gamepadDevice->handleButtonValueChange(negativeButton,
                                                       floatValue > 0.0F,
                                                       floatValue);
            }
            else
            {
                float floatValue = 2.0F * (newValue - min) / range - 1.0F;

                if (floatValue > 0.0F)
                {
                    gamepadDevice->handleButtonValueChange(positiveButton,
                                                           floatValue > THUMB_DEADZONE,
                                                           floatValue);
                }
                else if (floatValue < 0.0F)
                {
                    gamepadDevice->handleButtonValueChange(negativeButton,
                                                           -floatValue > THUMB_DEADZONE,
                                                           -floatValue);
                }
                else // thumbstick is 0
                {
                    if (oldValue > newValue)
                        gamepadDevice->handleButtonValueChange(positiveButton, false, 0.0F);
                    else
                        gamepadDevice->handleButtonValueChange(negativeButton, false, 0.0F);
                }
            }
        }
    } // namespace input
} // namespace ouzel
