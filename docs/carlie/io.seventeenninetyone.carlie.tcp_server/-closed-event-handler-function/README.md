[carlie](../../index.md) / [io.seventeenninetyone.carlie.tcp_server](../index.md) / [ClosedEventHandlerFunction](./index.md)

# ClosedEventHandlerFunction

`@FunctionalInterface interface ClosedEventHandlerFunction`

The functional interface for a function used as an event handler for when a
server has closed or when a server’s connections have closed.

**Author**
Jay B.

**See Also**

[io.seventeenninetyone.carlie.TcpServer.onceClosed](../../io.seventeenninetyone.carlie/-tcp-server/once-closed.md)

[io.seventeenninetyone.carlie.TcpServer.Connection.onceClosed](../../io.seventeenninetyone.carlie/-tcp-server/-connection/once-closed.md)

### Functions

| Name | Summary |
|---|---|
| [handle](handle.md) | `abstract fun handle(): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html) |
| [invoke](invoke.md) | `open operator fun invoke(): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html) |
