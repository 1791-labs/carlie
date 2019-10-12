[carlie](../../index.md) / [io.seventeenninetyone.carlie.tcp_server](../index.md) / [ClientConnectedEventHandlerFunction](./index.md)

# ClientConnectedEventHandlerFunction

`@FunctionalInterface interface ClientConnectedEventHandlerFunction`

The functional interface for a function used as an event handler for incoming
connections on a server.

**Author**
Jay B.

**See Also**

[io.seventeenninetyone.carlie.TcpServer.onClientConnected](../../io.seventeenninetyone.carlie/-tcp-server/on-client-connected.md)

### Functions

| Name | Summary |
|---|---|
| [handle](handle.md) | `abstract fun handle(connection: Connection): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html) |
| [invoke](invoke.md) | `open operator fun invoke(connection: Connection): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html) |
