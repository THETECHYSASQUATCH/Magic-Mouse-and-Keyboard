/*++

Module Name:

    queue.c

Abstract:

    This file contains the queue entry points and callbacks.

Environment:

    Kernel-mode Driver Framework

--*/

#include "driver.h"
#include "queue.tmh"
#include "Public.h"
#include "ReportFormats.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, AppleKBNMouseQueueInitialize)
#endif

// Forward declarations for parser functions
VOID AppleParseMouseReport(_In_reads_bytes_(len) const UCHAR* raw, size_t len, _Out_ PAPPLE_MOUSE_PARSED_REPORT out);
VOID AppleParseKeyboardReport(_In_reads_bytes_(len) const UCHAR* raw, size_t len, _Out_ PAPPLE_KEYBOARD_PARSED_REPORT out);
VOID AppleParseTrackpadReport(_In_reads_bytes_(len) const UCHAR* raw, size_t len, _Out_ PAPPLE_TRACKPAD_PARSED_REPORT out);

NTSTATUS
AppleKBNMouseQueueInitialize(
    _In_ WDFDEVICE Device
    )
/*++

Routine Description:

     The I/O dispatch callbacks for the frameworks device object
     are configured in this function.

--*/
{
    WDFQUEUE queue;
    NTSTATUS status;
    WDF_IO_QUEUE_CONFIG queueConfig;

    PAGED_CODE();

    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(
         &queueConfig,
        WdfIoQueueDispatchParallel
        );

    queueConfig.EvtIoDeviceControl = AppleKBNMouseEvtIoDeviceControl;
    queueConfig.EvtIoStop = AppleKBNMouseEvtIoStop;

    status = WdfIoQueueCreate(
                 Device,
                 &queueConfig,
                 WDF_NO_OBJECT_ATTRIBUTES,
                 &queue
                 );

    if(!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "WdfIoQueueCreate failed %!STATUS!", status);
        return status;
    }

    return status;
}

// Suppress unused parameter warnings for inline helpers by referencing params.
#define REF_PARAM(x) (void)(x)

static NTSTATUS CopyConfigToPublic(_In_ const APPLE_INPUT_CONFIG* src, _Out_ APPLE_INPUT_CONFIG_PUBLIC* dst)
{
    REF_PARAM(src); REF_PARAM(dst);
    dst->version = src->version;
    dst->enableNaturalScroll = src->enableNaturalScroll;
    dst->enableInertia = src->enableInertia;
    dst->tapToClick = src->tapToClick;
    dst->trackingSpeed = src->trackingSpeed;
    RtlZeroMemory(dst->reserved, sizeof(dst->reserved));
    return STATUS_SUCCESS;
}

static NTSTATUS CopyConfigFromPublic(_In_ const APPLE_INPUT_CONFIG_PUBLIC* src, _Out_ APPLE_INPUT_CONFIG* dst)
{
    REF_PARAM(src); REF_PARAM(dst);
    if (src->version != 1) return STATUS_INVALID_PARAMETER;
    dst->version = src->version;
    dst->enableNaturalScroll = src->enableNaturalScroll;
    dst->enableInertia = src->enableInertia;
    dst->tapToClick = src->tapToClick;
    dst->trackingSpeed = src->trackingSpeed;
    return STATUS_SUCCESS;
}

static NTSTATUS CopyBatteryToPublic(_In_ const APPLE_BATTERY_STATUS* src, _Out_ APPLE_BATTERY_STATUS_PUBLIC* dst)
{
    REF_PARAM(src); REF_PARAM(dst);
    dst->levelPercent = src->levelPercent;
    dst->isCharging = src->isCharging;
    RtlZeroMemory(dst->reserved, sizeof(dst->reserved));
    return STATUS_SUCCESS;
}

static NTSTATUS CopyBatteryFromPublic(_In_ const APPLE_BATTERY_STATUS_PUBLIC* src, _Out_ APPLE_BATTERY_STATUS* dst)
{
    REF_PARAM(src); REF_PARAM(dst);
    if (src->levelPercent > 100 && src->levelPercent != 0xFFFFFFFF) return STATUS_INVALID_PARAMETER;
    dst->levelPercent = src->levelPercent;
    dst->isCharging = src->isCharging;
    return STATUS_SUCCESS;
}

VOID
AppleKBNMouseEvtIoDeviceControl(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t OutputBufferLength,
    _In_ size_t InputBufferLength,
    _In_ ULONG IoControlCode
    )
{
    TraceEvents(TRACE_LEVEL_INFORMATION, 
                TRACE_QUEUE, 
                "%!FUNC! Queue 0x%p, Request 0x%p Out %Iu In %Iu Ioctl %x", 
                Queue, Request, OutputBufferLength, InputBufferLength, IoControlCode);

    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

    PDEVICE_CONTEXT devCtx = DeviceGetContext(WdfIoQueueGetDevice(Queue));

    PUCHAR inBuf = NULL; size_t inSize = 0; PVOID outBuf = NULL; size_t outSize = 0;

    switch (IoControlCode) {
    case IOCTL_APPLEKBNMOUSE_PARSE_MOUSE_REPORT: {
        NTSTATUS status = WdfRequestRetrieveInputBuffer(Request, 1, (PVOID*)&inBuf, &inSize);
        if (!NT_SUCCESS(status)) { WdfRequestComplete(Request, status); return; }
        status = WdfRequestRetrieveOutputBuffer(Request, sizeof(APPLE_MOUSE_PARSED_REPORT), &outBuf, &outSize);
        if (!NT_SUCCESS(status)) { WdfRequestComplete(Request, status); return; }
        AppleParseMouseReport(inBuf, inSize, (PAPPLE_MOUSE_PARSED_REPORT)outBuf);
        WdfRequestSetInformation(Request, sizeof(APPLE_MOUSE_PARSED_REPORT));
        WdfRequestComplete(Request, STATUS_SUCCESS);
        return; }
    case IOCTL_APPLEKBNMOUSE_PARSE_KEYBOARD_REPORT: {
        NTSTATUS status = WdfRequestRetrieveInputBuffer(Request, 1, (PVOID*)&inBuf, &inSize);
        if (!NT_SUCCESS(status)) { WdfRequestComplete(Request, status); return; }
        status = WdfRequestRetrieveOutputBuffer(Request, sizeof(APPLE_KEYBOARD_PARSED_REPORT), &outBuf, &outSize);
        if (!NT_SUCCESS(status)) { WdfRequestComplete(Request, status); return; }
        AppleParseKeyboardReport(inBuf, inSize, (PAPPLE_KEYBOARD_PARSED_REPORT)outBuf);
        WdfRequestSetInformation(Request, sizeof(APPLE_KEYBOARD_PARSED_REPORT));
        WdfRequestComplete(Request, STATUS_SUCCESS);
        return; }
    case IOCTL_APPLEKBNMOUSE_PARSE_TRACKPAD_REPORT: {
        NTSTATUS status = WdfRequestRetrieveInputBuffer(Request, 1, (PVOID*)&inBuf, &inSize);
        if (!NT_SUCCESS(status)) { WdfRequestComplete(Request, status); return; }
        status = WdfRequestRetrieveOutputBuffer(Request, sizeof(APPLE_TRACKPAD_PARSED_REPORT), &outBuf, &outSize);
        if (!NT_SUCCESS(status)) { WdfRequestComplete(Request, status); return; }
        AppleParseTrackpadReport(inBuf, inSize, (PAPPLE_TRACKPAD_PARSED_REPORT)outBuf);
        WdfRequestSetInformation(Request, sizeof(APPLE_TRACKPAD_PARSED_REPORT));
        WdfRequestComplete(Request, STATUS_SUCCESS);
        return; }
    case IOCTL_APPLEKBNMOUSE_GET_CONFIG: {
        NTSTATUS status = WdfRequestRetrieveOutputBuffer(Request, sizeof(APPLE_INPUT_CONFIG_PUBLIC), &outBuf, &outSize);
        if (!NT_SUCCESS(status)) { WdfRequestComplete(Request, status); return; }
        ExAcquireFastMutex(&devCtx->ConfigMutex);
        CopyConfigToPublic(&devCtx->currentConfig, (PAPPLE_INPUT_CONFIG_PUBLIC)outBuf);
        ExReleaseFastMutex(&devCtx->ConfigMutex);
        WdfRequestSetInformation(Request, sizeof(APPLE_INPUT_CONFIG_PUBLIC));
        WdfRequestComplete(Request, STATUS_SUCCESS);
        return; }
    case IOCTL_APPLEKBNMOUSE_SET_CONFIG: {
        NTSTATUS status = WdfRequestRetrieveInputBuffer(Request, sizeof(APPLE_INPUT_CONFIG_PUBLIC), (PVOID*)&inBuf, &inSize);
        if (!NT_SUCCESS(status) || inSize < sizeof(APPLE_INPUT_CONFIG_PUBLIC)) { WdfRequestComplete(Request, STATUS_BUFFER_TOO_SMALL); return; }
        APPLE_INPUT_CONFIG_PUBLIC* pubCfg = (APPLE_INPUT_CONFIG_PUBLIC*)inBuf;
        APPLE_INPUT_CONFIG tmp;
        status = CopyConfigFromPublic(pubCfg, &tmp);
        if (!NT_SUCCESS(status)) { WdfRequestComplete(Request, status); return; }
        ExAcquireFastMutex(&devCtx->ConfigMutex);
        devCtx->currentConfig = tmp;
        ExReleaseFastMutex(&devCtx->ConfigMutex);
        WdfRequestComplete(Request, STATUS_SUCCESS);
        return; }
    case IOCTL_APPLEKBNMOUSE_GET_BATTERY: {
        NTSTATUS status = WdfRequestRetrieveOutputBuffer(Request, sizeof(APPLE_BATTERY_STATUS_PUBLIC), &outBuf, &outSize);
        if (!NT_SUCCESS(status)) { WdfRequestComplete(Request, status); return; }
        ExAcquireFastMutex(&devCtx->ConfigMutex);
        CopyBatteryToPublic(&devCtx->batteryStatus, (PAPPLE_BATTERY_STATUS_PUBLIC)outBuf);
        ExReleaseFastMutex(&devCtx->ConfigMutex);
        WdfRequestSetInformation(Request, sizeof(APPLE_BATTERY_STATUS_PUBLIC));
        WdfRequestComplete(Request, STATUS_SUCCESS);
        return; }
    case IOCTL_APPLEKBNMOUSE_UPDATE_BATTERY: {
        NTSTATUS status = WdfRequestRetrieveInputBuffer(Request, sizeof(APPLE_BATTERY_STATUS_PUBLIC), (PVOID*)&inBuf, &inSize);
        if (!NT_SUCCESS(status) || inSize < sizeof(APPLE_BATTERY_STATUS_PUBLIC)) { WdfRequestComplete(Request, STATUS_BUFFER_TOO_SMALL); return; }
        APPLE_BATTERY_STATUS_PUBLIC* pubBat = (APPLE_BATTERY_STATUS_PUBLIC*)inBuf;
        APPLE_BATTERY_STATUS tmpBat;
        status = CopyBatteryFromPublic(pubBat, &tmpBat);
        if (!NT_SUCCESS(status)) { WdfRequestComplete(Request, status); return; }
        ExAcquireFastMutex(&devCtx->ConfigMutex);
        devCtx->batteryStatus = tmpBat;
        ExReleaseFastMutex(&devCtx->ConfigMutex);
        WdfRequestComplete(Request, STATUS_SUCCESS);
        return; }
    default:
        WdfRequestComplete(Request, STATUS_INVALID_DEVICE_REQUEST);
        return;
    }
}

VOID
AppleKBNMouseEvtIoStop(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ ULONG ActionFlags
)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, 
                TRACE_QUEUE, 
                "%!FUNC! Queue 0x%p, Request 0x%p ActionFlags %d", 
                Queue, Request, ActionFlags);
    UNREFERENCED_PARAMETER(Queue);
    UNREFERENCED_PARAMETER(Request);
    UNREFERENCED_PARAMETER(ActionFlags);
}
