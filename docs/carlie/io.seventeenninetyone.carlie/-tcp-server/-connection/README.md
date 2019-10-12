[carlie](../../../index.md) / [io.seventeenninetyone.carlie](../../index.md) / [TcpServer](../index.md) / [Connection](./index.md)

# Connection

`interface Connection : `[`AsynchronousByteChannel`](https://docs.oracle.com/javase/8/docs/api/java/nio/channels/AsynchronousByteChannel.html)`, `[`AutoCloseable`](https://docs.oracle.com/javase/8/docs/api/java/lang/AutoCloseable.html)

An interface for a server’s connections.

**Author**
Jay B.

**See Also**

[io.seventeenninetyone.carlie.TcpServer.onClientConnected](../on-client-connected.md)

[io.seventeenninetyone.carlie.tcp_server.ClientConnectedEventHandlerFunction](../../../io.seventeenninetyone.carlie.tcp_server/-client-connected-event-handler-function/index.md)

[java.nio.channels.AsynchronousByteChannel](https://docs.oracle.com/javase/8/docs/api/java/nio/channels/AsynchronousByteChannel.html)

[java.lang.AutoCloseable](https://docs.oracle.com/javase/8/docs/api/java/lang/AutoCloseable.html)

### Properties

| Name | Summary |
|---|---|
| [id](id.md) | Get the unique identifier of the connection.`abstract val id: `[`UUID`](https://docs.oracle.com/javase/8/docs/api/java/util/UUID.html) |
| [isKeepAliveEnabled](is-keep-alive-enabled.md) | Check if the TCP keep-alive functionality is enabled for the connection.`abstract val isKeepAliveEnabled: `[`Boolean`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-boolean/index.html) |
| [localAddress](local-address.md) | Get the local address of the connection.`abstract val localAddress: Address?` |
| [remoteAddress](remote-address.md) | Get the remote address of the connection.`abstract val remoteAddress: Address?` |
| [server](server.md) | Get the server.`abstract val server: `[`TcpServer`](../index.md) |

### Functions

| Name | Summary |
|---|---|
| [asInputStream](as-input-stream.md) | Get a view of the connection as a [java.io.InputStream](https://docs.oracle.com/javase/8/docs/api/java/io/InputStream.html), for reading.`abstract fun asInputStream(): `[`InputStream`](https://docs.oracle.com/javase/8/docs/api/java/io/InputStream.html) |
| [asOutputStream](as-output-stream.md) | Get a view of the connection as a [java.io.OutputStream](https://docs.oracle.com/javase/8/docs/api/java/io/OutputStream.html), for writing.`abstract fun asOutputStream(): `[`OutputStream`](https://docs.oracle.com/javase/8/docs/api/java/io/OutputStream.html) |
| [close](close.md) | Close the connection and free up any resources that may be held by it.`abstract fun close(): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html) |
| [disableKeepAlive](disable-keep-alive.md) | Disable the TCP keep-alive functionality for the connection.`abstract fun disableKeepAlive(): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html) |
| [enableKeepAlive](enable-keep-alive.md) | Enable the TCP keep-alive functionality for the connection.`abstract fun enableKeepAlive(initialDelay: `[`Int`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-int/index.html)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)<br>`abstract fun enableKeepAlive(initialDelay: `[`UInt`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-u-int/index.html)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html) |
| [onceClosed](once-closed.md) | Attach an event handler for when the connection has closed.`abstract fun onceClosed(handler: `[`ClosedEventHandlerFunction`](../../../io.seventeenninetyone.carlie.tcp_server/-closed-event-handler-function/index.md)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html) |
| [onErrorOccurred](on-error-occurred.md) | Attach an event handler for errors occurring on the connection.`abstract fun onErrorOccurred(handler: `[`ErrorOccurredEventHandlerFunction`](../../../io.seventeenninetyone.carlie.tcp_server/-error-occurred-event-handler-function/index.md)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html) |
| [read](read.md) | Read (asynchronously) from the connection’s underlying stream.`abstract fun read(destinationBuffer: `[`ByteBuffer`](https://docs.oracle.com/javase/8/docs/api/java/nio/ByteBuffer.html)`): `[`Future`](https://docs.oracle.com/javase/8/docs/api/java/util/concurrent/Future.html)`<`[`Int`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-int/index.html)`>`<br>`abstract fun <A> read(destinationBuffer: `[`ByteBuffer`](https://docs.oracle.com/javase/8/docs/api/java/nio/ByteBuffer.html)`, attachment: A, handler: `[`CompletionHandler`](https://docs.oracle.com/javase/8/docs/api/java/nio/channels/CompletionHandler.html)`<`[`Int`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-int/index.html)`, in A>): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html) |
| [toString](to-string.md) | Produce a string representation of the connection and its state.`abstract fun toString(): `[`String`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-string/index.html) |
| [write](write.md) | Write (asynchronously) to the connection’s underlying stream.`abstract fun write(sourceBuffer: `[`ByteBuffer`](https://docs.oracle.com/javase/8/docs/api/java/nio/ByteBuffer.html)`): `[`Future`](https://docs.oracle.com/javase/8/docs/api/java/util/concurrent/Future.html)`<`[`Int`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-int/index.html)`>`<br>`abstract fun <A> write(sourceBuffer: `[`ByteBuffer`](https://docs.oracle.com/javase/8/docs/api/java/nio/ByteBuffer.html)`, attachment: A, handler: `[`CompletionHandler`](https://docs.oracle.com/javase/8/docs/api/java/nio/channels/CompletionHandler.html)`<`[`Int`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-int/index.html)`, in A>): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html) |
