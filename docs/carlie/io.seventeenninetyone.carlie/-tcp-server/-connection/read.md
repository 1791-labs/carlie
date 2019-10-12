[carlie](../../../index.md) / [io.seventeenninetyone.carlie](../../index.md) / [TcpServer](../index.md) / [Connection](index.md) / [read](./read.md)

# read

`abstract fun read(destinationBuffer: `[`ByteBuffer`](https://docs.oracle.com/javase/8/docs/api/java/nio/ByteBuffer.html)`): `[`Future`](https://docs.oracle.com/javase/8/docs/api/java/util/concurrent/Future.html)`<`[`Int`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-int/index.html)`>`
`abstract fun <A> read(destinationBuffer: `[`ByteBuffer`](https://docs.oracle.com/javase/8/docs/api/java/nio/ByteBuffer.html)`, attachment: A, handler: `[`CompletionHandler`](https://docs.oracle.com/javase/8/docs/api/java/nio/channels/CompletionHandler.html)`<`[`Int`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-int/index.html)`, in A>): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)

Read (asynchronously) from the connection’s underlying stream.

**See Also**

[java.nio.channels.AsynchronousByteChannel.read](https://docs.oracle.com/javase/8/docs/api/java/nio/channels/AsynchronousByteChannel.html#read(java.nio.ByteBuffer, A, java.nio.channels.CompletionHandler<java.lang.Integer,? super A>))

