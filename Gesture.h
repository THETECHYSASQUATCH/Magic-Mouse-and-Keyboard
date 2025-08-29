#pragma once
#include <wdm.h>
#include "ReportFormats.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GESTURE_ENGINE_CONTEXT {
    ULONG Placeholder; // future state (scroll inertia accumulators, etc.)
} GESTURE_ENGINE_CONTEXT, *PGESTURE_ENGINE_CONTEXT;

void GestureEngineInitialize(_Out_ PGESTURE_ENGINE_CONTEXT ctx);
void GestureEngineProcessTrackpad(_Inout_ PGESTURE_ENGINE_CONTEXT ctx, _In_ const APPLE_TRACKPAD_PARSED_REPORT* rpt);

#ifdef __cplusplus
}
#endif
