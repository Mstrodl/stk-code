#ifdef __SWITCH__
#include "input/switch_pad.hpp"
#include "input/device_manager.hpp"
#include "input/gamepad_config.hpp"
#include "input/gamepad_device.hpp"
#include "utils/string_utils.hpp"

#define BUTTON_COUNT 31
#define AXIS_COUNT 2
#define START_ID 32

enum ButtonTypes {
    BUTTON_A = 0,
    BUTTON_B,
    BUTTON_X,
    BUTTON_Y,
    BUTTON_ZL,
    BUTTON_ZR,
    BUTTON_L,
    BUTTON_R,
    BUTTON_MINUS,
    BUTTON_PLUS,
    BUTTON_LEFT,
    BUTTON_UP,
    BUTTON_RIGHT,
    BUTTON_DOWN,
    BUTTON_STICK,
    BUTTON_STICK_RIGHT,
};

SwitchPad::SwitchPad(InputManager* inputManager, int id) {
    if (id == 9)
    {
        id = HidNpadIdType_Handheld;
    }
    else if (id == 8)
    {
        id = HidNpadIdType_Other;
    }
    printf("Creating SwitchPad!\n");
    m_inputManager = input_manager;
    m_id = id;
    padInitializeWithMask(&m_pad, 1UL << id);

    m_event.EventType = irr::EET_JOYSTICK_INPUT_EVENT;
    m_event.JoystickEvent.Joystick = START_ID + id;
    m_event.JoystickEvent.AxisChanged = 0;

    DeviceManager* deviceManager = m_inputManager->getDeviceManager();
    std::string data = "Player ";
    data.append(StringUtils::toString(id).c_str());

    GamepadConfig* gamepadConfig = NULL;

    // It's okay that we reuse 32 from the wiimotes, because switch has no wiiuse
    deviceManager->getConfigForGamepad(START_ID, "Switch Controller",
                                        &gamepadConfig);
    gamepadConfig->setNumberOfButtons(BUTTON_COUNT);
    gamepadConfig->setNumberOfAxis(AXIS_COUNT);

    gamepadConfig->setBinding(PA_STEER_LEFT,   Input::IT_STICKMOTION, 0, Input::AD_NEGATIVE);
    gamepadConfig->setBinding(PA_STEER_RIGHT,  Input::IT_STICKMOTION, 0, Input::AD_POSITIVE);
    gamepadConfig->setBinding(PA_ACCEL,        Input::IT_STICKBUTTON, BUTTON_A);
    gamepadConfig->setBinding(PA_BRAKE,        Input::IT_STICKMOTION, BUTTON_B);
    gamepadConfig->setBinding(PA_FIRE,         Input::IT_STICKBUTTON, BUTTON_X);
    gamepadConfig->setBinding(PA_NITRO,        Input::IT_STICKBUTTON, BUTTON_L);
    gamepadConfig->setBinding(PA_DRIFT,        Input::IT_STICKBUTTON, BUTTON_R);
    gamepadConfig->setBinding(PA_RESCUE,       Input::IT_STICKBUTTON, BUTTON_STICK);
    gamepadConfig->setBinding(PA_LOOK_BACK,    Input::IT_STICKBUTTON, BUTTON_Y);
    gamepadConfig->setBinding(PA_PAUSE_RACE,   Input::IT_STICKBUTTON, BUTTON_PLUS);

    gamepadConfig->setBinding(PA_MENU_UP,      Input::IT_STICKBUTTON, BUTTON_UP);
    gamepadConfig->setBinding(PA_MENU_DOWN,    Input::IT_STICKBUTTON, BUTTON_DOWN);
    gamepadConfig->setBinding(PA_MENU_LEFT,    Input::IT_STICKBUTTON, BUTTON_LEFT);
    gamepadConfig->setBinding(PA_MENU_RIGHT,   Input::IT_STICKBUTTON, BUTTON_RIGHT);
    gamepadConfig->setBinding(PA_MENU_SELECT,  Input::IT_STICKBUTTON, BUTTON_A);
    gamepadConfig->setBinding(PA_MENU_CANCEL,  Input::IT_STICKBUTTON, BUTTON_B);
    
    deviceManager->addGamepad(new GamePadDevice(START_ID + id,
                                                data.c_str(),
                                                AXIS_COUNT,
                                                BUTTON_COUNT,
                                                gamepadConfig));

    for (int i = 0; i < SEvent::SJoystickEvent::NUMBER_OF_AXES; ++i)
    {
        m_event.JoystickEvent.Axis[i] = 0;
    }
}

void SwitchPad::update() {
    padUpdate(&m_pad);

    HidAnalogStickState rightStick = padGetStickPos(&m_pad, 1);
    HidAnalogStickState leftStick = padGetStickPos(&m_pad, 0);

    uint64_t buttons = padGetButtons(&m_pad);
    m_event.JoystickEvent.ButtonStates = 0;
#define ADD_BUTTON(buttonMask, id) if (buttons & buttonMask)\
        m_event.JoystickEvent.ButtonStates |= 1 << id

#define SET_AXIS(IRR_AXIS, VALUE) if (m_event.JoystickEvent.Axis[irr::SEvent::SJoystickEvent::IRR_AXIS] != VALUE)\
        m_event.JoystickEvent.AxisChanged |= 1 << irr::SEvent::SJoystickEvent::IRR_AXIS;\
    m_event.JoystickEvent.Axis[irr::SEvent::SJoystickEvent::IRR_AXIS] = VALUE;

    ADD_BUTTON(HidNpadButton_StickL, BUTTON_STICK);
    ADD_BUTTON(HidNpadButton_StickR, BUTTON_STICK_RIGHT);
    ADD_BUTTON(HidNpadButton_A, BUTTON_A);
    ADD_BUTTON(HidNpadButton_B, BUTTON_B);
    ADD_BUTTON(HidNpadButton_X, BUTTON_X);
    ADD_BUTTON(HidNpadButton_Y, BUTTON_Y);
    ADD_BUTTON(HidNpadButton_L, BUTTON_L);
    ADD_BUTTON(HidNpadButton_R, BUTTON_R);
    ADD_BUTTON(HidNpadButton_ZL, BUTTON_ZL);
    ADD_BUTTON(HidNpadButton_ZR, BUTTON_ZR);
    
    ADD_BUTTON(HidNpadButton_AnyLeft, BUTTON_LEFT);
    ADD_BUTTON(HidNpadButton_AnyUp, BUTTON_UP);
    ADD_BUTTON(HidNpadButton_AnyDown, BUTTON_DOWN);
    ADD_BUTTON(HidNpadButton_AnyRight, BUTTON_RIGHT);
        
    SET_AXIS(AXIS_X, leftStick.x);
    SET_AXIS(AXIS_Y, -leftStick.y);
    SET_AXIS(AXIS_Z, rightStick.x);
    SET_AXIS(AXIS_R, -rightStick.y);

    ADD_BUTTON(HidNpadButton_Minus, BUTTON_MINUS);
    ADD_BUTTON(HidNpadButton_Plus, BUTTON_PLUS);
    
    m_inputManager->input(m_event);
}
#endif
