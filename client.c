#include <Windows.h>
#include <winternl.h>
#include <ntstatus.h>
#include <stdio.h>

#pragma comment(lib, "ntdll.lib")

NTSTATUS NTAPI NtWriteFile(
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

NTSTATUS OpenMailslot(PCWSTR mailslotName, HANDLE* pHandle) {
    UNICODE_STRING name;
    OBJECT_ATTRIBUTES objAttr;
    IO_STATUS_BLOCK ioStatus;

    RtlInitUnicodeString(&name, mailslotName);
    InitializeObjectAttributes(&objAttr, &name, OBJ_CASE_INSENSITIVE, NULL, NULL);

    NTSTATUS status = NtCreateFile(pHandle, GENERIC_WRITE, &objAttr, &ioStatus, NULL, 0, FILE_SHARE_READ, FILE_OPEN, 0, NULL, 0);
    if (!NT_SUCCESS(status)) {
        printf("[-] Error opening native mailslot server: %x\n", status);
    } else printf("[+] connected to native mailslot server\n");
    return status;
}

NTSTATUS SendServerMessage(HANDLE hMailslot, const char* message) {
    IO_STATUS_BLOCK ioStatus;

    NTSTATUS status = NtWriteFile(hMailslot, NULL, NULL, NULL, &ioStatus, (PVOID)message, strlen(message), NULL, NULL);
    if (!NT_SUCCESS(status)) {
        printf("[-] Error sending message: %x\n", status);
    }
    else {
        printf("[+] Message '%s' sent!\n", message);
    }
    return status;
}

int main() {
    printf("[i] Initiate writer communication protocol for IPC (Pyramidyon).\r\n\r\n");

    HANDLE hMailslot;
    NTSTATUS status = OpenMailslot(L"\\Device\\Mailslot\\Pyramidyon", &hMailslot);
    if (NT_SUCCESS(status)) {
        const char* message = "Hello server.";
        SendServerMessage(hMailslot, message);
        NtClose(hMailslot);
    }
    return 0;
}
