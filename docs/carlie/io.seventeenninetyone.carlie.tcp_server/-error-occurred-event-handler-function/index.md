[carlie](../../index.md) / [io.seventeenninetyone.carlie.tcp_server](../index.md) / [ErrorOccurredEventHandlerFunction](./index.md)

# ErrorOccurredEventHandlerFunction

`@FunctionalInterface interface ErrorOccurredEventHandlerFunction`

The functional interface for a function used as an event handler for errors
occurring on a server or errors occurring on a server’s connections.

**Author**
Jay B.

**See Also**

[io.seventeenninetyone.carlie.TcpServer.onErrorOccurred](../../io.seventeenninetyone.carlie/-tcp-server/on-error-occurred.md)

[io.seventeenninetyone.carlie.TcpServer.Connection.onErrorOccurred](../../io.seventeenninetyone.carlie/-tcp-server/-connection/on-error-occurred.md)

### Functions

| Name | Summary |
|---|---|
| [handle](handle.md) | `abstract fun handle(error: `[`Throwable`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-throwable/index.html)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html) |
| [invoke](invoke.md) | `open operator fun invoke(error: `[`Throwable`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-throwable/index.html)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html) |
