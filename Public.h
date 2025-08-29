/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

#pragma once

#include <initguid.h>

// Interface Guid so that apps can find the device and talk to it.
DEFINE_GUID (GUID_DEVINTERFACE_AppleKBNMouse,
    0xcc892c49,0x2c65,0x4cda,0xbe,0xbe,0xdf,0x8a,0xb1,0x9a,0xfb,0x4c);
// {cc892c49-2c65-4cda-bebe-df8ab19afb4c}

// IOCTL base index
#define APPLEKBNMOUSE_IOCTL_INDEX          0x800

// Parsing IOCTLs (input: raw Apple report, output: parsed structure)
#define IOCTL_APPLEKBNMOUSE_PARSE_MOUSE_REPORT       CTL_CODE(FILE_DEVICE_UNKNOWN, APPLEKBNMOUSE_IOCTL_INDEX + 0, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLEKBNMOUSE_PARSE_KEYBOARD_REPORT    CTL_CODE(FILE_DEVICE_UNKNOWN, APPLEKBNMOUSE_IOCTL_INDEX + 1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLEKBNMOUSE_PARSE_TRACKPAD_REPORT    CTL_CODE(FILE_DEVICE_UNKNOWN, APPLEKBNMOUSE_IOCTL_INDEX + 2, METHOD_BUFFERED, FILE_ANY_ACCESS)

// Configuration & Battery IOCTLs
// GET: output buffer receives APPLE_INPUT_CONFIG
// SET: input buffer supplies APPLE_INPUT_CONFIG
#define IOCTL_APPLEKBNMOUSE_GET_CONFIG              CTL_CODE(FILE_DEVICE_UNKNOWN, APPLEKBNMOUSE_IOCTL_INDEX + 10, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLEKBNMOUSE_SET_CONFIG              CTL_CODE(FILE_DEVICE_UNKNOWN, APPLEKBNMOUSE_IOCTL_INDEX + 11, METHOD_BUFFERED, FILE_ANY_ACCESS)

// Battery: GET returns APPLE_BATTERY_STATUS, UPDATE allows user-mode service to push latest values
#define IOCTL_APPLEKBNMOUSE_GET_BATTERY             CTL_CODE(FILE_DEVICE_UNKNOWN, APPLEKBNMOUSE_IOCTL_INDEX + 12, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLEKBNMOUSE_UPDATE_BATTERY          CTL_CODE(FILE_DEVICE_UNKNOWN, APPLEKBNMOUSE_IOCTL_INDEX + 13, METHOD_BUFFERED, FILE_ANY_ACCESS)

// Public user-mode visible structures (keep simple types to avoid kernel headers)
typedef struct _APPLE_INPUT_CONFIG_PUBLIC {
    unsigned long version;            // structure version
    unsigned long enableNaturalScroll; // 0/1
    unsigned long enableInertia;      // 0/1
    unsigned long tapToClick;         // 0/1
    unsigned long trackingSpeed;      // 0..100 logical scale
    unsigned long reserved[8];
} APPLE_INPUT_CONFIG_PUBLIC, *PAPPLE_INPUT_CONFIG_PUBLIC;

typedef struct _APPLE_BATTERY_STATUS_PUBLIC {
    unsigned long levelPercent;   // 0..100 (or 0xFFFFFFFF unknown)
    unsigned long isCharging;     // boolean
    unsigned long reserved[2];
} APPLE_BATTERY_STATUS_PUBLIC, *PAPPLE_BATTERY_STATUS_PUBLIC;
