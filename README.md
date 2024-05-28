# IPC Communication Using Native Mailslots @Pyramidyon

### PoC:
https://github.com/pyramidyon/using-NtCreateMailslotFile-for-IPC/assets/88564775/32d6746e-f490-43f3-acf2-0470804aa9c3

This program utilizes Windows Native API functions to facilitate Inter-Process Communication (IPC) through mailslots. Leveraging lower-level system functions, this IPC mechanism offers a robust and efficient method for data exchange between processes. Mailslots support a 'many writers/one reader' protocol, making them ideal for scenarios where multiple clients need to communicate with a single server.

### Concept
This Proof of Concept (PoC) demonstrates how modular malware, which is plugin-based, does not need to require each plugin to have its own Command and Control (C2) communication protocol. Instead, they can implement an IPC mechanism allowing a mailslots server to handle the data and transmit it to the C2, while the plugins merely need a mailslots client to send data.

## How It Works
- **Creation of Mailslot**: The program begins by creating a mailslot with `NtCreateMailslotFile`, configured for message-based communication.
- **Event Handling**: Utilizes `NtCreateEvent` to manage asynchronous read operations, enabling the system to signal when new data is available.
- **Reading Messages**: Implements an efficient message reading loop that waits for events signaled by `NtReadFile`, indicating that new data has arrived.

## Proof of Concept
- **Setup Mailslot Communication**: Demonstrates how to establish a mailslot for IPC.
- **Asynchronous Data Handling**: Shows how to effectively manage asynchronous reads using events.
- **Message Exchange**: Provides an example of how messages are exchanged between processes.
