#include "elamsample.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#endif // ALLOC_PRAGMA

//
// Trace level to use for kernel debugger DbgPrintEx output.
//

#define ELAMSAMPLE_TRACE_LEVEL DPFLTR_TRACE_LEVEL

//
// Callback handle returned by IoRegisterBootDriverCallback.
//

static PVOID g_IoRegisterBootDriverCallbackHandle = NULL;

//
// Current status update type from the callback.
//

static BDCB_STATUS_UPDATE_TYPE g_CurrentBcdCallbackContextType =
                                    BdCbStatusPrepareForDependencyLoad;

NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    )
{
    WDF_OBJECT_ATTRIBUTES Attributes;
    WDF_DRIVER_CONFIG Config;
    WDFDRIVER Driver;
    NTSTATUS Status;

    DbgPrintEx(DPFLTR_IHVDRIVER_ID,
               DPFLTR_ERROR_LEVEL,
               "ElamSample is being initialized.\r\n");

    //
    // Initialize a non-PnP driver with the framework.
    //

    WDF_DRIVER_CONFIG_INIT(&Config, WDF_NO_EVENT_CALLBACK);

    Config.DriverInitFlags |= WdfDriverInitNonPnpDriver;

    //
    // Non-PnP drivers must register an unload routine.
    //

    Config.EvtDriverUnload = ElamSampleEvtDriverUnload;

    //
    // Create a framework driver object.
    //

    WDF_OBJECT_ATTRIBUTES_INIT(&Attributes);

    Status = WdfDriverCreate(DriverObject,
                             RegistryPath,
                             &Attributes,
                             &Config,
                             &Driver);

    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    //
    // Register for the boot driver callback.
    //

    g_IoRegisterBootDriverCallbackHandle = IoRegisterBootDriverCallback(
                                                ElamSampleBootDriverCallback,
                                                NULL);

    if (g_IoRegisterBootDriverCallbackHandle == NULL)
    {
        return STATUS_UNSUCCESSFUL;
    }

    return STATUS_SUCCESS;
}

VOID
ElamSampleEvtDriverUnload(
    _In_ WDFDRIVER Driver
    )

{
    UNREFERENCED_PARAMETER(Driver);

    if (g_IoRegisterBootDriverCallbackHandle != NULL)
    {
        IoUnregisterBootDriverCallback(g_IoRegisterBootDriverCallbackHandle);
        g_IoRegisterBootDriverCallbackHandle = NULL;
    }

    DbgPrintEx(DPFLTR_IHVDRIVER_ID,
                DPFLTR_ERROR_LEVEL,
               "ElamSample is being unloaded.\r\n");
}
