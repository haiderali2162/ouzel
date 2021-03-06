// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif
#include <algorithm>
#include <functional>
#include "Widgets.hpp"
#include "BMFont.hpp"
#include "core/Engine.hpp"
#include "events/EventHandler.hpp"
#include "events/EventDispatcher.hpp"
#include "math/Color.hpp"
#include "scene/Layer.hpp"
#include "scene/Camera.hpp"

namespace ouzel
{
    namespace gui
    {
        Button::Button():
            eventHandler(EventHandler::PRIORITY_MAX + 1)
        {
            eventHandler.uiHandler = std::bind(&Button::handleUI, this, std::placeholders::_1);
            engine->getEventDispatcher().addEventHandler(eventHandler);

            pickable = true;
        }

        Button::Button(const std::string& normalImage,
                       const std::string& selectedImage,
                       const std::string& pressedImage,
                       const std::string& disabledImage,
                       const std::string& label,
                       const std::string& font,
                       float fontSize,
                       Color initLabelColor,
                       Color initLabelSelectedColor,
                       Color initLabelPressedColor,
                       Color initLabelDisabledColor):
            eventHandler(EventHandler::PRIORITY_MAX + 1),
            labelColor(initLabelColor),
            labelSelectedColor(initLabelSelectedColor),
            labelPressedColor(initLabelPressedColor),
            labelDisabledColor(initLabelDisabledColor)
        {
            eventHandler.uiHandler = std::bind(&Button::handleUI, this, std::placeholders::_1);
            engine->getEventDispatcher().addEventHandler(eventHandler);

            if (!normalImage.empty())
            {
                normalSprite = std::make_unique<scene::SpriteRenderer>();
                normalSprite->init(normalImage);
                addComponent(normalSprite.get());
            }

            if (!selectedImage.empty())
            {
                selectedSprite = std::make_unique<scene::SpriteRenderer>();
                selectedSprite->init(selectedImage);
                addComponent(selectedSprite.get());
            }

            if (!pressedImage.empty())
            {
                pressedSprite = std::make_unique<scene::SpriteRenderer>();
                pressedSprite->init(pressedImage);
                addComponent(pressedSprite.get());
            }

            if (!disabledImage.empty())
            {
                disabledSprite = std::make_unique<scene::SpriteRenderer>();
                disabledSprite->init(disabledImage);
                addComponent(disabledSprite.get());
            }

            if (!label.empty())
            {
                labelDrawable = std::make_unique<scene::TextRenderer>(font, fontSize, label);
                labelDrawable->setColor(labelColor);
                addComponent(labelDrawable.get());
            }

            pickable = true;

            updateSprite();
        }

        void Button::setEnabled(bool newEnabled)
        {
            Widget::setEnabled(newEnabled);

            selected = false;
            pointerOver = false;
            pressed = false;

            updateSprite();
        }

        void Button::setSelected(bool newSelected)
        {
            Widget::setSelected(newSelected);

            updateSprite();
        }

        bool Button::handleUI(const UIEvent& event)
        {
            if (!enabled) return false;

            if (event.actor == this)
            {
                switch (event.type)
                {
                    case Event::Type::ActorEnter:
                    {
                        pointerOver = true;
                        updateSprite();
                        break;
                    }
                    case Event::Type::ActorLeave:
                    {
                        pointerOver = false;
                        updateSprite();
                        break;
                    }
                    case Event::Type::ActorPress:
                    {
                        pressed = true;
                        updateSprite();
                        break;
                    }
                    case Event::Type::ActorRelease:
                    case Event::Type::ActorClick:
                    {
                        if (pressed)
                        {
                            pressed = false;
                            updateSprite();
                        }
                        break;
                    }
                    default:
                        return false;
                }
            }

            return false;
        }

        void Button::updateSprite()
        {
            if (normalSprite) normalSprite->setHidden(true);
            if (selectedSprite) selectedSprite->setHidden(true);
            if (pressedSprite) pressedSprite->setHidden(true);
            if (disabledSprite) disabledSprite->setHidden(true);

            if (enabled)
            {
                if (pressed && pointerOver && pressedSprite)
                    pressedSprite->setHidden(false);
                else if (selected && selectedSprite)
                    selectedSprite->setHidden(false);
                else if (normalSprite)
                    normalSprite->setHidden(false);

                if (labelDrawable)
                {
                    if (pressed && pointerOver)
                        labelDrawable->setColor(labelPressedColor);
                    else if (selected)
                        labelDrawable->setColor(labelSelectedColor);
                    else
                        labelDrawable->setColor(labelColor);
                }
            }
            else // disabled
            {
                if (disabledSprite)
                    disabledSprite->setHidden(false);
                else if (normalSprite)
                    normalSprite->setHidden(false);

                if (labelDrawable)
                    labelDrawable->setColor(labelDisabledColor);
            }
        }

        CheckBox::CheckBox(const std::string& normalImage,
                           const std::string& selectedImage,
                           const std::string& pressedImage,
                           const std::string& disabledImage,
                           const std::string& tickImage):
            eventHandler(EventHandler::PRIORITY_MAX + 1)
        {
            eventHandler.uiHandler = std::bind(&CheckBox::handleUI, this, std::placeholders::_1);
            engine->getEventDispatcher().addEventHandler(eventHandler);

            if (!normalImage.empty())
            {
                normalSprite = std::make_unique<scene::SpriteRenderer>();
                normalSprite->init(normalImage);
                addComponent(normalSprite.get());
            }

            if (!selectedImage.empty())
            {
                selectedSprite = std::make_unique<scene::SpriteRenderer>();
                selectedSprite->init(selectedImage);
                addComponent(selectedSprite.get());
            }

            if (!pressedImage.empty())
            {
                pressedSprite = std::make_unique<scene::SpriteRenderer>();
                pressedSprite->init(pressedImage);
                addComponent(pressedSprite.get());
            }

            if (!disabledImage.empty())
            {
                disabledSprite = std::make_unique<scene::SpriteRenderer>();
                disabledSprite->init(disabledImage);
                addComponent(disabledSprite.get());
            }

            if (!tickImage.empty())
            {
                tickSprite = std::make_unique<scene::SpriteRenderer>();
                tickSprite->init(tickImage);
                addComponent(tickSprite.get());
            }

            pickable = true;

            updateSprite();
        }

        void CheckBox::setEnabled(bool newEnabled)
        {
            Widget::setEnabled(newEnabled);

            selected = false;
            pointerOver = false;
            pressed = false;

            updateSprite();
        }

        void CheckBox::setChecked(bool newChecked)
        {
            checked = newChecked;

            updateSprite();
        }

        bool CheckBox::handleUI(const UIEvent& event)
        {
            if (!enabled) return false;

            if (event.actor == this)
            {
                switch (event.type)
                {
                    case Event::Type::ActorEnter:
                    {
                        pointerOver = true;
                        updateSprite();
                        break;
                    }
                    case Event::Type::ActorLeave:
                    {
                        pointerOver = false;
                        updateSprite();
                        break;
                    }
                    case Event::Type::ActorPress:
                    {
                        pressed = true;
                        updateSprite();
                        break;
                    }
                    case Event::Type::ActorRelease:
                    {
                        if (pressed)
                        {
                            pressed = false;
                            updateSprite();
                        }
                        break;
                    }
                    case Event::Type::ActorClick:
                    {
                        pressed = false;
                        checked = !checked;
                        updateSprite();

                        auto changeEvent = std::make_unique<UIEvent>();
                        changeEvent->type = Event::Type::WidgetChange;
                        changeEvent->actor = event.actor;
                        engine->getEventDispatcher().dispatchEvent(std::move(changeEvent));
                        break;
                    }
                    default:
                        return false;
                }
            }

            return false;
        }

        void CheckBox::updateSprite()
        {
            if (enabled)
            {
                if (pressed && pointerOver && pressedSprite)
                    pressedSprite->setHidden(false);
                else if (selected && selectedSprite)
                    selectedSprite->setHidden(false);
                else if (normalSprite)
                    normalSprite->setHidden(false);
            }
            else
            {
                if (disabledSprite)
                    disabledSprite->setHidden(false);
                else if (normalSprite)
                    normalSprite->setHidden(false);
            }

            if (tickSprite)
                tickSprite->setHidden(!checked);
        }

        ComboBox::ComboBox()
        {
            pickable = true;
        }

        EditBox::EditBox()
        {
            pickable = true;
        }

        void EditBox::setValue(const std::string& newValue)
        {
            value = newValue;
        }

        Label::Label(const std::string& initText,
                     const std::string& fontFile,
                     float fontSize,
                     Color color,
                     const Vector2F& textAnchor):
            text(initText),
            labelDrawable(std::make_shared<scene::TextRenderer>(fontFile, fontSize, text, color, textAnchor))
        {
            addComponent(labelDrawable.get());
            labelDrawable->setText(text);

            pickable = true;
        }

        void Label::setText(const std::string& newText)
        {
            text = newText;
            labelDrawable->setText(text);
        }

        Menu::Menu():
            eventHandler(EventHandler::PRIORITY_MAX + 1)
        {
            eventHandler.keyboardHandler = std::bind(&Menu::handleKeyboard, this, std::placeholders::_1);
            eventHandler.gamepadHandler = std::bind(&Menu::handleGamepad, this, std::placeholders::_1);
            eventHandler.uiHandler = std::bind(&Menu::handleUI, this, std::placeholders::_1);
        }

        void Menu::enter()
        {
            engine->getEventDispatcher().addEventHandler(eventHandler);
        }

        void Menu::leave()
        {
            eventHandler.remove();
        }

        void Menu::setEnabled(bool newEnabled)
        {
            Widget::setEnabled(newEnabled);

            if (enabled)
            {
                if (!selectedWidget && !widgets.empty())
                    selectWidget(widgets.front());
            }
            else
            {
                selectedWidget = nullptr;

                for (Widget* childWidget : widgets)
                    childWidget->setSelected(false);
            }
        }

        void Menu::addWidget(Widget* widget)
        {
            addChild(widget);

            if (widget)
            {
                if (widget->menu)
                    widget->menu->removeChild(widget);

                widget->menu = this;
                widgets.push_back(widget);

                if (!selectedWidget)
                    selectWidget(widget);
            }
        }

        bool Menu::removeChild(const Actor* actor)
        {
            auto i = std::find(widgets.begin(), widgets.end(), actor);

            if (i != widgets.end())
            {
                Widget* widget = *i;
                widget->menu = nullptr;

                widgets.erase(i);
            }

            if (selectedWidget == actor)
                selectWidget(nullptr);

            if (!Actor::removeChild(actor))
                return false;

            return true;
        }

        void Menu::selectWidget(Widget* widget)
        {
            if (!enabled) return;

            selectedWidget = nullptr;

            for (Widget* childWidget : widgets)
            {
                if (childWidget == widget)
                {
                    selectedWidget = widget;
                    childWidget->setSelected(true);
                }
                else
                    childWidget->setSelected(false);
            }
        }

        void Menu::selectNextWidget()
        {
            if (!enabled) return;

            auto firstWidgetIterator = selectedWidget ?
            std::find(widgets.begin(), widgets.end(), selectedWidget) :
            widgets.end();

            auto widgetIterator = firstWidgetIterator;

            do
            {
                if (widgetIterator == widgets.end())
                    widgetIterator = widgets.begin();
                else
                    ++widgetIterator;

                if (widgetIterator != widgets.end() && (*widgetIterator)->isEnabled())
                {
                    selectWidget(*widgetIterator);
                    break;
                }
            }
            while (widgetIterator != firstWidgetIterator);
        }

        void Menu::selectPreviousWidget()
        {
            if (!enabled) return;

            auto firstWidgetIterator = selectedWidget ?
            std::find(widgets.begin(), widgets.end(), selectedWidget) :
            widgets.end();

            auto widgetIterator = firstWidgetIterator;

            do
            {
                if (widgetIterator == widgets.begin()) widgetIterator = widgets.end();
                if (widgetIterator != widgets.begin()) --widgetIterator;

                if (widgetIterator != widgets.end() && (*widgetIterator)->isEnabled())
                {
                    selectWidget(*widgetIterator);
                    break;
                }
            }
            while (widgetIterator != firstWidgetIterator);
        }

        bool Menu::handleKeyboard(const KeyboardEvent& event)
        {
            if (!enabled) return false;

            if (event.type == Event::Type::KeyboardKeyPress && !widgets.empty())
            {
                switch (event.key)
                {
                    case input::Keyboard::Key::Left:
                    case input::Keyboard::Key::Up:
                        selectPreviousWidget();
                        break;
                    case input::Keyboard::Key::Right:
                    case input::Keyboard::Key::Down:
                        selectNextWidget();
                        break;
                    case input::Keyboard::Key::Enter:
                    case input::Keyboard::Key::Space:
                    case input::Keyboard::Key::Select:
                    {
                        if (selectedWidget)
                        {
                            auto clickEvent = std::make_unique<UIEvent>();
                            clickEvent->type = Event::Type::ActorClick;
                            clickEvent->actor = selectedWidget;
                            clickEvent->position = Vector2F(selectedWidget->getPosition());
                            engine->getEventDispatcher().dispatchEvent(std::move(clickEvent));
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            return false;
        }

        bool Menu::handleGamepad(const GamepadEvent& event)
        {
            if (!enabled) return false;

            if (event.type == Event::Type::GamepadButtonChange)
            {
                if (event.button == input::Gamepad::Button::DpadLeft ||
                    event.button == input::Gamepad::Button::DpadUp)
                {
                    if (!event.previousPressed && event.pressed) selectPreviousWidget();
                }
                else if (event.button == input::Gamepad::Button::DpadRight ||
                         event.button == input::Gamepad::Button::DpadDown)
                {
                    if (!event.previousPressed && event.pressed) selectNextWidget();
                }
                else if (event.button == input::Gamepad::Button::LeftThumbLeft ||
                         event.button == input::Gamepad::Button::LeftThumbUp)
                {
                    if (event.previousValue < 0.6F && event.value > 0.6F) selectPreviousWidget();
                }
                else if (event.button == input::Gamepad::Button::LeftThumbRight ||
                         event.button == input::Gamepad::Button::LeftThumbDown)
                {
                    if (event.previousValue < 0.6F && event.value > 0.6F) selectNextWidget();
                }
#if !defined(__APPLE__) || (!TARGET_OS_IOS && !TARGET_OS_TV) // on iOS and tvOS menu items ar selected with a SELECT button
                else if (event.button == input::Gamepad::Button::FaceBottom)
                {
                    if (!event.previousPressed && event.pressed && selectedWidget)
                    {
                        auto clickEvent = std::make_unique<UIEvent>();
                        clickEvent->type = Event::Type::ActorClick;
                        clickEvent->actor = selectedWidget;
                        clickEvent->position = Vector2F(selectedWidget->getPosition());
                        engine->getEventDispatcher().dispatchEvent(std::move(clickEvent));
                    }
                }
#endif
            }

            return false;
        }

        bool Menu::handleUI(const UIEvent& event)
        {
            if (!enabled) return false;

            if (event.type == Event::Type::ActorEnter)
                if (std::find(widgets.begin(), widgets.end(), event.actor) != widgets.end())
                    selectWidget(static_cast<Widget*>(event.actor));

            return false;
        }

        RadioButton::RadioButton()
        {
            pickable = true;
        }

        void RadioButton::setEnabled(bool newEnabled)
        {
            Widget::setEnabled(newEnabled);

            selected = false;
            pointerOver = false;
            pressed = false;
        }

        void RadioButton::setChecked(bool newChecked)
        {
            checked = newChecked;
        }

        RadioButtonGroup::RadioButtonGroup()
        {
        }

        ScrollArea::ScrollArea()
        {
        }

        ScrollBar::ScrollBar()
        {
            pickable = true;
        }

        SlideBar::SlideBar()
        {
            pickable = true;
        }
    } // namespace gui
} // namespace ouzel
