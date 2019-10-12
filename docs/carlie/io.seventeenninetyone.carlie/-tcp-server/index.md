[carlie](../../index.md) / [io.seventeenninetyone.carlie](../index.md) / [TcpServer](./index.md)

# TcpServer

`class TcpServer : `[`AutoCloseable`](https://docs.oracle.com/javase/8/docs/api/java/lang/AutoCloseable.html)

This class is for creating TCP servers.

**Author**
Jay B.

**See Also**

[java.lang.AutoCloseable](https://docs.oracle.com/javase/8/docs/api/java/lang/AutoCloseable.html)

### Types

| Name | Summary |
|---|---|
| [Address](-address/index.md) | An interface for internet addresses returned by a server or a server’s connections.`interface Address` |
| [Connection](-connection/index.md) | An interface for a server’s connections.`interface Connection : `[`AsynchronousByteChannel`](https://docs.oracle.com/javase/8/docs/api/java/nio/channels/AsynchronousByteChannel.html)`, `[`AutoCloseable`](https://docs.oracle.com/javase/8/docs/api/java/lang/AutoCloseable.html) |

### Constructors

| Name | Summary |
|---|---|
| [&lt;init&gt;](-init-.md) | Create a new server.`TcpServer()` |

### Properties

| Name | Summary |
|---|---|
| [address](address.md) | Get the address of the server.`val address: Address?` |
| [connectionsCount](connections-count.md) | Get the number of active connections on the server.`val connectionsCount: `[`Int`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-int/index.html) |
| [isListening](is-listening.md) | Check if the server is currently listening for connections.`var isListening: `[`Boolean`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-boolean/index.html) |

### Functions

| Name | Summary |
|---|---|
| [listen](listen.md) | Listen for connections.`fun listen(): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)<br>`fun listen(hostName: `[`String`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-string/index.html)` = ""): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)<br>`fun listen(hostName: `[`String`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-string/index.html)` = "", listeningEventHandler: `[`ListeningEventHandlerFunction`](../../io.seventeenninetyone.carlie.tcp_server/-listening-event-handler-function/index.md)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)<br>`fun listen(listeningEventHandler: `[`ListeningEventHandlerFunction`](../../io.seventeenninetyone.carlie.tcp_server/-listening-event-handler-function/index.md)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)<br>`fun listen(hostName: `[`String`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-string/index.html)` = "", port: `[`UShort`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-u-short/index.html)` = 0u): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)<br>`fun listen(hostName: `[`String`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-string/index.html)` = "", port: `[`Int`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-int/index.html)` = 0): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)<br>`fun listen(hostName: `[`String`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-string/index.html)` = "", port: `[`UInt`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-u-int/index.html)` = 0u): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)<br>`fun listen(hostName: `[`String`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-string/index.html)` = "", port: `[`UShort`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-u-short/index.html)` = 0u, listeningEventHandler: `[`ListeningEventHandlerFunction`](../../io.seventeenninetyone.carlie.tcp_server/-listening-event-handler-function/index.md)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)<br>`fun listen(hostName: `[`String`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-string/index.html)` = "", port: `[`Int`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-int/index.html)` = 0, listeningEventHandler: `[`ListeningEventHandlerFunction`](../../io.seventeenninetyone.carlie.tcp_server/-listening-event-handler-function/index.md)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)<br>`fun listen(hostName: `[`String`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-string/index.html)` = "", port: `[`UInt`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-u-int/index.html)` = 0u, listeningEventHandler: `[`ListeningEventHandlerFunction`](../../io.seventeenninetyone.carlie.tcp_server/-listening-event-handler-function/index.md)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)<br>`fun listen(port: `[`UShort`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-u-short/index.html)` = 0u): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)<br>`fun listen(port: `[`Int`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-int/index.html)` = 0): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)<br>`fun listen(port: `[`UInt`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-u-int/index.html)` = 0u): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)<br>`fun listen(port: `[`UShort`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-u-short/index.html)` = 0u, listeningEventHandler: `[`ListeningEventHandlerFunction`](../../io.seventeenninetyone.carlie.tcp_server/-listening-event-handler-function/index.md)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)<br>`fun listen(port: `[`Int`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-int/index.html)` = 0, listeningEventHandler: `[`ListeningEventHandlerFunction`](../../io.seventeenninetyone.carlie.tcp_server/-listening-event-handler-function/index.md)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)<br>`fun listen(port: `[`UInt`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-u-int/index.html)` = 0u, listeningEventHandler: `[`ListeningEventHandlerFunction`](../../io.seventeenninetyone.carlie.tcp_server/-listening-event-handler-function/index.md)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html) |
| [onceClosed](once-closed.md) | Attach an event handler for when the server has closed.`fun onceClosed(handler: `[`ClosedEventHandlerFunction`](../../io.seventeenninetyone.carlie.tcp_server/-closed-event-handler-function/index.md)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html) |
| [onceListening](once-listening.md) | Attach an event handler for when the server has started to listen.`fun onceListening(handler: `[`ListeningEventHandlerFunction`](../../io.seventeenninetyone.carlie.tcp_server/-listening-event-handler-function/index.md)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html) |
| [onClientConnected](on-client-connected.md) | Attach an event handler for incoming connections on the server.`fun onClientConnected(handler: `[`ClientConnectedEventHandlerFunction`](../../io.seventeenninetyone.carlie.tcp_server/-client-connected-event-handler-function/index.md)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html) |
| [onErrorOccurred](on-error-occurred.md) | Attach an event handler for errors occurring on the server.`fun onErrorOccurred(handler: `[`ErrorOccurredEventHandlerFunction`](../../io.seventeenninetyone.carlie.tcp_server/-error-occurred-event-handler-function/index.md)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html) |
| [start](start.md) | Start the server.`fun start(): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html) |
| [stop](stop.md) | Stop the server; *i.e.*, close all active connections and stop listening for connections.`fun stop(): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html) |
| [toString](to-string.md) | Produce a string representation of the server and its state.`fun toString(): `[`String`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-string/index.html) |
