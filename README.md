# Chat Application

This chat application allows multiple clients to connect to a server and exchange messages in real time. It's built in C, utilizing sockets for networking and pthreads for handling multiple clients simultaneously.

## Features

- **Multi-client Support**: The server can handle multiple clients concurrently.
- **Real-time Communication**: Clients can send and receive messages instantly.
- **Non-blocking Server**: The server is implemented in a non-blocking way, ensuring it can handle client connections and messages efficiently.
- **Signal Handling**: The server gracefully shuts down upon receiving a SIGINT signal.

## Structure

The application is divided into two main parts:

- **Client**: Represents a chat client that can send and receive messages.
- **Server**: Handles incoming client connections and broadcasts messages to all connected clients.

Each client and server has its structure defined (`ClientType` and `ServerType`) for managing their respective states and threads.

## Compilation and Usage

### Prerequisites

- GCC compiler
- Linux environment (due to the usage of pthreads and certain system calls)

### Compilation

Navigate to the project directory and run the following command to compile the server and client:

```bash
make all
```

This will produce two executable files: `chatapp_server` and `chatapp_client`.

### Running the Server

To start the server, run:

```bash
./chatapp_server
```

The server will start and listen for incoming client connections.

### Running a Client

To connect a client to the server, run:

```bash
./chatapp_client <ServerIPAddress>
```

Replace `<ServerIPAddress>` with the IP address of the machine where the server is running.

## Implementation Details

### Server

- Initializes the server socket and sets it to non-blocking mode.
- Listens for incoming client connections and handles them in separate threads.
- Broadcasts messages received from any client to all other connected clients.
- Handles server shutdown and resource cleanup gracefully.

### Client

- Connects to the server using the provided IP address.
- Runs two threads: one for sending messages and another for receiving messages.
- Handles user input and displays incoming messages in real-time.

## Error Handling

The application includes basic error handling for system calls and user inputs. Errors are logged to the console with an appropriate message.

## Cleanup

Both the client and server ensure that all allocated resources (like sockets and memory) are properly released when the application is closed or when an unexpected error occurs.

---

This `README` provides a basic overview and guide on how to compile and use the chat application. Feel free to add more sections as needed, such as "Known Issues", "Future Improvements", or "License".
