#pragma once
// Kernel-mode IOCTL / structures for virtual Apple Precision Touchpad (PTP)
// Use kernel headers instead of winioctl.h to avoid undefined DWORD/WORD when compiling KMDF/WDM.
#include <ntddk.h>
#include <devioctl.h>

// {8C3F9B78-3C9E-46E8-A3A9-8AB2B45A0623}
DEFINE_GUID(GUID_DEVINTERFACE_APPLEPTP,
0x8c3f9b78, 0x3c9e, 0x46e8, 0xa3, 0xa9, 0x8a, 0xb2, 0xb4, 0x5a, 0x06, 0x23);

#define APPLEPTP_DEVICE_TYPE     0x9000
#define IOCTL_APPLEPTP_INJECT_FRAME CTL_CODE(APPLEPTP_DEVICE_TYPE, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define APPLEPTP_MAX_CONTACTS 5

#pragma pack(push, 1)
typedef struct _APPLEPTP_CONTACT {
    USHORT ContactId;
    USHORT X;
    USHORT Y;
    USHORT Width;
    USHORT Height;
    UCHAR  TipSwitch;
    UCHAR  Confidence;
} APPLEPTP_CONTACT, *PAPPLEPTP_CONTACT;

typedef struct _APPLEPTP_FRAME {
    UCHAR  ReportId;
    UCHAR  ContactCount;
    UCHAR  Buttons;
    APPLEPTP_CONTACT Contacts[APPLEPTP_MAX_CONTACTS];
} APPLEPTP_FRAME, *PAPPLEPTP_FRAME;
#pragma pack(pop)