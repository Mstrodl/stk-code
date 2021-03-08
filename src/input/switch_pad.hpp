#ifdef __SWITCH__
extern "C" {
#define Event libnx_Event
#define u64 libnx_u64
#define u32 libnx_u32
#define s64 libnx_s64
#define s32 libnx_s32
#include <switch/runtime/pad.h>
#undef u64
#undef u32
#undef s64
#undef s32
#undef Event
}

#include "IEventReceiver.h"
#include "input/input_manager.hpp"

class SwitchPad {
private:
    PadState m_pad;
    int m_id;
    InputManager* m_inputManager;
    irr::SEvent m_event;
public:
    SwitchPad(InputManager* inputManager, int id);
    void update();
};
#endif
