[carlie](../index.md) / [io.seventeenninetyone.carlie.tcp_server](./index.md)

## Package io.seventeenninetyone.carlie.tcp_server

### Types

| Name | Summary |
|---|---|
| [ClientConnectedEventHandlerFunction](-client-connected-event-handler-function/index.md) | The functional interface for a function used as an event handler for incoming connections on a server.`interface ClientConnectedEventHandlerFunction` |
| [ClosedEventHandlerFunction](-closed-event-handler-function/index.md) | The functional interface for a function used as an event handler for when a server has closed or when a server’s connections have closed.`interface ClosedEventHandlerFunction` |
| [ErrorOccurredEventHandlerFunction](-error-occurred-event-handler-function/index.md) | The functional interface for a function used as an event handler for errors occurring on a server or errors occurring on a server’s connections.`interface ErrorOccurredEventHandlerFunction` |
| [ListeningEventHandlerFunction](-listening-event-handler-function/index.md) | The functional interface for a function used as an event handler for when a server has started to listen on a bound address.`interface ListeningEventHandlerFunction` |

### Exceptions

| Name | Summary |
|---|---|
| [InvalidPortException](-invalid-port-exception/index.md) | An exception thrown when [io.seventeenninetyone.carlie.TcpServer.listen](../io.seventeenninetyone.carlie/-tcp-server/listen.md) is called with an invalid port.`class InvalidPortException : `[`IllegalArgumentException`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-illegal-argument-exception/index.html) |
| [ServerAlreadyListeningException](-server-already-listening-exception/index.md) | An exception thrown when [io.seventeenninetyone.carlie.TcpServer.listen](../io.seventeenninetyone.carlie/-tcp-server/listen.md) is called on a server that’s already listening for connections.`class ServerAlreadyListeningException : `[`Exception`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-exception/index.html) |
| [ServerClosedException](-server-closed-exception/index.md) | An exception thrown when an operation isn’t supported on a closed server.`class ServerClosedException : `[`Exception`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-exception/index.html) |
| [UvException](-uv-exception/index.md) | An exception thrown when a libuv-related error occurs.`class UvException : `[`Exception`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-exception/index.html) |
