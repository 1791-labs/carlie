[carlie](../../index.md) / [io.seventeenninetyone.carlie.tcp_server](../index.md) / [ListeningEventHandlerFunction](./index.md)

# ListeningEventHandlerFunction

`@FunctionalInterface interface ListeningEventHandlerFunction`

The functional interface for a function used as an event handler for when a
server has started to listen on a bound address.

**Author**
Jay B.

**See Also**

[io.seventeenninetyone.carlie.TcpServer.onceListening](../../io.seventeenninetyone.carlie/-tcp-server/once-listening.md)

### Functions

| Name | Summary |
|---|---|
| [handle](handle.md) | `abstract fun handle(): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html) |
| [invoke](invoke.md) | `open operator fun invoke(): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html) |
