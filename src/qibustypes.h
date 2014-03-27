#ifndef __Q_IBUS_TYPES_H_
#define __Q_IBUS_TYPES_H_

#include <QMetaType>

namespace IBus {

typedef enum
{
    ShiftMask    = 1 << 0,
    LockMask     = 1 << 1,
    ControlMask  = 1 << 2,
    Mod1Mask     = 1 << 3,
    Mod2Mask     = 1 << 4,
    Mod3Mask     = 1 << 5,
    Mod4Mask     = 1 << 6,
    Mod5Mask     = 1 << 7,
    Button1Mask  = 1 << 8,
    Button2Mask  = 1 << 9,
    Button3Mask  = 1 << 10,
    Button4Mask  = 1 << 11,
    Button5Mask  = 1 << 12,

    /* The next few modifiers are used by XKB, so we skip to the end.
    * Bits 15 - 23 are currently unused. Bit 29 is used internally.
    *
    **/

    /* ibus mask */
    HandledMask  = 1 << 24,
    ForwardMask  = 1 << 25,
    IgnoredMask  = ForwardMask,

    SuperMask    = 1 << 26,
    HyperMask    = 1 << 27,
    MetaMask     = 1 << 28,

    ReleaseMask  = 1 << 30,

    ModifierMask = 0x5c001fff
} ModifierType;

typedef enum {
    CapPreeditText       = 1 << 0,
    CapAuxiliaryText     = 1 << 1,
    CapLookupTable       = 1 << 2,
    CapFocus             = 1 << 3,
    CapProperty          = 1 << 4,
    CapSurroundingText   = 1 << 5,
} Capabilite;

typedef enum {
    OrientationHorizontal   = 0,
    OrientationVertical     = 1,
    OrientationSystem       = 2,
} Orientation;

typedef enum {
    EnginePreeditClear      = 0,
    EnginePreeditCommit     = 1,
} PreeditFocusMode;

typedef struct {
    uint x;
    uint y;
    uint width;
    uint height;
} Rectangle;

};

#endif

