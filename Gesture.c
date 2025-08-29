#include "Gesture.h"

void GestureEngineInitialize(_Out_ PGESTURE_ENGINE_CONTEXT ctx) {
    ctx->Placeholder = 0;
}

void GestureEngineProcessTrackpad(_Inout_ PGESTURE_ENGINE_CONTEXT ctx, _In_ const APPLE_TRACKPAD_PARSED_REPORT* rpt) {
    (void)ctx; (void)rpt; // stub
    // Future: accumulate velocities, classify swipe vs pinch, compute inertia.
}
