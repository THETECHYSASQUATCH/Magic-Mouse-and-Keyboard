#include "driver.h"
#include "ReportFormats.h"
#include "trace.h"
#include <string.h>

// Media key translation table (Apple usage -> HID Consumer Control usage)
static const struct { UINT16 apple; UINT16 win; } mediaKeyMap[] = {
    {0xE0, 0xE2}, // Mute
    {0xE1, 0xE9}, // Volume Up
    {0xE2, 0xEA}, // Volume Down
    {0xB0, 0xB5}, // Scan Next Track
    {0xB1, 0xB6}, // Scan Previous Track
    {0xCD, 0xCD}, // Play/Pause
    {0xE3, 0xE3}, // Stop
    {0xE4, 0xE4}, // Eject (repurpose)
    {0xF1, 0xF1}, // Brightness Up (custom mapping)
    {0xF2, 0xF2}, // Brightness Down (custom mapping)
    // Add more as needed
};

static UINT16 TranslateAppleMediaKey(UINT16 appleUsage) {
    for (unsigned i = 0; i < sizeof(mediaKeyMap)/sizeof(mediaKeyMap[0]); ++i) {
        if (mediaKeyMap[i].apple == appleUsage) return mediaKeyMap[i].win;
    }
    return 0;
}

// Track Fn key state and remap top-row keys
static void RemapFnLayer(const UINT16* in, UINT16* out, UINT32 modifiers, int invertFn) {
    // If Fn is held (or invertFn is set), map top-row to F1-F12, else to media
    for (int i = 0; i < APPLE_KBD_MAX_KEYS; ++i) {
        UINT16 usage = in[i];
        if (usage >= 0x3A && usage <= 0x45) { // F1-F12
            if ((modifiers & 0x01) || invertFn) // Fn held or inverted
                out[i] = usage; // F1-F12
            else {
                // Map to media
                UINT16 media = TranslateAppleMediaKey(usage);
                out[i] = media ? media : usage;
            }
        } else {
            out[i] = usage;
        }
    }
}

// Touch/gesture detection for Magic Mouse
static void DetectMouseGesture(const UCHAR* raw, size_t len, PAPPLE_MOUSE_PARSED_REPORT out) {
    // Placeholder: Apple Magic Mouse reports need reverse engineering
    // Example: if raw[3] == 0x01, treat as swipe left
    if (len > 3) {
        switch (raw[3]) {
            case 0x01: out->gesture = 1; out->swipe = 1; break; // swipe left
            case 0x02: out->gesture = 2; out->swipe = 2; break; // swipe right
            case 0x03: out->gesture = 3; out->tap = 1; break;   // tap
            default: out->gesture = 0; out->tap = 0; out->swipe = 0; break;
        }
    }
}

// Touch/gesture detection for Magic Trackpad
static void DetectTrackpadGesture(const UCHAR* raw, size_t len, PAPPLE_TRACKPAD_PARSED_REPORT out) {
    // Placeholder: Apple Trackpad reports need reverse engineering
    // Example: if raw[10] == 0x01, treat as swipe up
    if (len > 10) {
        switch (raw[10]) {
            case 0x01: out->swipe = 3; out->gestureFlags |= 0x01; break; // swipe up
            case 0x02: out->swipe = 4; out->gestureFlags |= 0x02; break; // swipe down
            case 0x03: out->tap = 1; break; // tap
            default: out->swipe = 0; out->tap = 0; break;
        }
    }
}

VOID AppleParseKeyboardReport(_In_reads_bytes_(len) const UCHAR* raw, size_t len, _Out_ PAPPLE_KEYBOARD_PARSED_REPORT out)
{
    RtlZeroMemory(out, sizeof(*out));
    if (len < 8) return;
    out->modifiers = raw[1];
    UINT16 temp[APPLE_KBD_MAX_KEYS] = {0};
    for (int i=0;i<APPLE_KBD_MAX_KEYS && (2+i)<(int)len;i++) {
        temp[i] = raw[2+i];
    }
    // Example: invertFn could be read from config
    int invertFn = 0;
    RemapFnLayer(temp, out->usage, out->modifiers, invertFn);
}

VOID AppleParseMouseReport(_In_reads_bytes_(len) const UCHAR* raw, size_t len, _Out_ PAPPLE_MOUSE_PARSED_REPORT out)
{
    RtlZeroMemory(out, sizeof(*out));
    if (len < 3) return;
    out->dx = (CHAR)raw[1];
    out->dy = (CHAR)raw[2];
    DetectMouseGesture(raw, len, out);
}

VOID AppleParseTrackpadReport(_In_reads_bytes_(len) const UCHAR* raw, size_t len, _Out_ PAPPLE_TRACKPAD_PARSED_REPORT out)
{
    RtlZeroMemory(out, sizeof(*out));
    // Real implementation would decode contacts, here just gesture stub
    DetectTrackpadGesture(raw, len, out);
}
