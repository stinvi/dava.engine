#pragma once

#include "Input/InputDevice.h"

namespace DAVA
{
class InputSystem;
namespace Private
{
struct MainDispatcherEvent;
}

/**
    \ingroup input
    Represents mouse input device.
*/
class MouseDevice final : public InputDevice
{
    friend class DeviceManager; // For creation

public:
    bool IsElementSupported(eInputElements elementId) const override;
    eDigitalElementStates GetDigitalElementState(eInputElements elementId) const override;
    AnalogElementState GetAnalogElementState(eInputElements elementId) const override;

    eInputElements GetFirstPressedButton() const;

private:
    MouseDevice(uint32 id);
    ~MouseDevice() override;
    MouseDevice(const MouseDevice&) = delete;

    bool HandleEvent(const Private::MainDispatcherEvent& e);
    void HandleMouseClick(const Private::MainDispatcherEvent& e);
    void HandleMouseWheel(const Private::MainDispatcherEvent& e);
    void HandleMouseMove(const Private::MainDispatcherEvent& e);

    void OnEndFrame();

private:
    InputSystem* inputSystem = nullptr;

    Array<eDigitalElementStates, INPUT_ELEMENTS_MOUSE_BUTTON_COUNT> buttons;
    AnalogElementState mousePosition;
    AnalogElementState mouseWheelDelta;
};

} // namespace DAVA
