#pragma once
#include <wdm.h>

// Mouse parsed report with gesture/touch support
typedef struct _APPLE_MOUSE_PARSED_REPORT {
    INT dx;
    INT dy;
    INT wheel;    // vertical scroll delta (high resolution target)
    INT hwheel;   // horizontal scroll delta
    UCHAR buttons; // bitfield (Left=1, Right=2, Middle=4, etc.)
    UCHAR gesture; // 0=none, 1=swipe left, 2=swipe right, 3=tap, etc.
    UCHAR tap;     // 1 if tap detected
    UCHAR swipe;   // 1=left, 2=right, 3=up, 4=down
} APPLE_MOUSE_PARSED_REPORT, *PAPPLE_MOUSE_PARSED_REPORT;

// Keyboard
#define APPLE_KBD_MAX_KEYS 6

typedef struct _APPLE_KEYBOARD_PARSED_REPORT {
    UINT32 modifiers; // bitmask: bit positions mapping to LeftCtrl, LeftShift, etc.
    UINT16 usage[APPLE_KBD_MAX_KEYS];  // HID usages of pressed keys (boot style)
} APPLE_KEYBOARD_PARSED_REPORT, *PAPPLE_KEYBOARD_PARSED_REPORT;

// Trackpad contact
typedef struct _APPLE_TRACKPAD_CONTACT {
    INT16 x;
    INT16 y;
    UINT8 id;
    UINT8 pressure; // pressure/size approximation
    UINT8 flags;     // tip/contact/palm bits
} APPLE_TRACKPAD_CONTACT, *PAPPLE_TRACKPAD_CONTACT;

#define APPLE_TRACKPAD_MAX_CONTACTS 10

typedef struct _APPLE_TRACKPAD_PARSED_REPORT {
    UINT8 contactCount;
    APPLE_TRACKPAD_CONTACT contacts[APPLE_TRACKPAD_MAX_CONTACTS];
    UINT8 gestureFlags; // bits for pinch/zoom/rotate/swipe classification
    INT scrollX; // high resolution scroll accumulator
    INT scrollY;
    UINT8 tap; // 1 if tap detected
    UINT8 swipe; // 1=left, 2=right, 3=up, 4=down
} APPLE_TRACKPAD_PARSED_REPORT, *PAPPLE_TRACKPAD_PARSED_REPORT;

// Configuration structure (shared with user-mode helper) for gesture preferences.
typedef struct _APPLE_INPUT_CONFIG {
    UINT32 version;            // structure version
    UINT32 enableNaturalScroll; // 0/1
    UINT32 enableInertia;      // 0/1
    UINT32 tapToClick;         // 0/1
    UINT32 trackingSpeed;      // 0..100 logical scale
    UINT32 reserved[8];
} APPLE_INPUT_CONFIG, *PAPPLE_INPUT_CONFIG;

// Battery status abstraction (per device)
typedef struct _APPLE_BATTERY_STATUS {
    UINT32 levelPercent;   // 0..100 (or 0xFFFFFFFF unknown)
    UINT32 isCharging;     // boolean
    UINT32 reserved[2];
} APPLE_BATTERY_STATUS, *PAPPLE_BATTERY_STATUS;
