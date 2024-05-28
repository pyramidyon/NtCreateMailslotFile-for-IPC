#include <Windows.h>
#include <winternl.h>
#include <ntstatus.h>
#include <stdio.h>

#pragma comment(lib, "ntdll.lib")

typedef enum _EVENT_TYPE
{
    NotificationEvent,
    SynchronizationEvent

} EVENT_TYPE;

NTSTATUS NTAPI NtReadFile(
    HANDLE FileHandle,
    HANDLE Event,
    PIO_APC_ROUTINE ApcRoutine,
    PVOID ApcContext,
    PIO_STATUS_BLOCK IoStatusBlock,
    PVOID Buffer,
    ULONG Length,
    PLARGE_INTEGER ByteOffset,
    PULONG Key
);

// Prototyping NtCreateMailslotFile and NtReadFile
NTSTATUS NTAPI NtCreateMailslotFile(
    PHANDLE FileHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PIO_STATUS_BLOCK IoStatusBlock,
    ULONG CreateOptions,
    ULONG MailslotQuota,
    ULONG MaximumMessageSize,
    PLARGE_INTEGER ReadTimeout
);


NTSTATUS NTAPI NtCreateEvent(
    PHANDLE EventHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    EVENT_TYPE EventType,
    BOOLEAN InitialState
);

NTSTATUS NTAPI NtWaitForSingleObject(
    HANDLE Handle,
    BOOLEAN Alertable,
    PLARGE_INTEGER Timeout
);

void ReadMessages(HANDLE hMailslot) {
    char buffer[1024]; //MAX LEN
    IO_STATUS_BLOCK ioStatus;
    HANDLE eventHandle;
    OBJECT_ATTRIBUTES objAttributes;
    NTSTATUS status;

    InitializeObjectAttributes(&objAttributes, NULL, 0, NULL, NULL);
    status = NtCreateEvent(&eventHandle, EVENT_MODIFY_STATE | SYNCHRONIZE, &objAttributes, SynchronizationEvent, FALSE);
    if (!NT_SUCCESS(status)) {
        printf("[-] Failed NtCreateEvent: %x\n", status);
        return;
    } else printf("[+] NtCreateEvent created\n");

    while (TRUE) {
        status = NtReadFile(hMailslot, eventHandle, NULL, NULL, &ioStatus, buffer, sizeof(buffer) - 1, NULL, NULL);
        if (status == STATUS_PENDING) {
            printf("[i] calling NtWaitForSingleObject to wait for message\n");
            NtWaitForSingleObject(eventHandle, FALSE, NULL);  // Wait indefinitely
            status = ioStatus.Status;
        } else printf("[i] Received Data NtReadFile\n");

        if (!NT_SUCCESS(status)) {
            printf("[-] Error reading from NtReadFile: %x\n", status);
            break;
        }

        if (ioStatus.Information > 0 && ioStatus.Information < sizeof(buffer)) {
            buffer[ioStatus.Information] = '\0';
            printf("[i] Message: %s\n", buffer);
        }
        else {
            printf("[-] Invalid read size or no data.\n");
        }
    }

    NtClose(eventHandle);
}

int main() { //Initiate a multi-writer, single-reader communication protocol for IPC.
    printf("[i] single-reader communication protocol for IPC (Pyramidyon).\r\n\r\n");

    HANDLE hMailslot;
    UNICODE_STRING name;
    OBJECT_ATTRIBUTES objAttr;
    IO_STATUS_BLOCK ioStatus;
    LARGE_INTEGER timeout;

    RtlInitUnicodeString(&name, L"\\Device\\Mailslot\\Pyramidyon");
    InitializeObjectAttributes(&objAttr, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);

    timeout.QuadPart = -1; // Indefinite wait
    NTSTATUS status = NtCreateMailslotFile(&hMailslot, GENERIC_READ | FILE_WRITE_ATTRIBUTES, &objAttr, &ioStatus, 0, 0, 0, &timeout);
    if (!NT_SUCCESS(status)) {
        printf("[-] Error creating NtCreateMailslotFile: %x\n", status);
        return 1;
    } else printf("[+] Created NtCreateMailslotFile\n");

    ReadMessages(hMailslot);
    NtClose(hMailslot);
    return 0;
}
