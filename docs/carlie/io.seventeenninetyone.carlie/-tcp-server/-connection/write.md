[carlie](../../../index.md) / [io.seventeenninetyone.carlie](../../index.md) / [TcpServer](../index.md) / [Connection](index.md) / [write](./write.md)

# write

`abstract fun write(sourceBuffer: `[`ByteBuffer`](https://docs.oracle.com/javase/8/docs/api/java/nio/ByteBuffer.html)`): `[`Future`](https://docs.oracle.com/javase/8/docs/api/java/util/concurrent/Future.html)`<`[`Int`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-int/index.html)`>`
`abstract fun <A> write(sourceBuffer: `[`ByteBuffer`](https://docs.oracle.com/javase/8/docs/api/java/nio/ByteBuffer.html)`, attachment: A, handler: `[`CompletionHandler`](https://docs.oracle.com/javase/8/docs/api/java/nio/channels/CompletionHandler.html)`<`[`Int`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-int/index.html)`, in A>): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)

Write (asynchronously) to the connection’s underlying stream.

**See Also**

[java.nio.channels.AsynchronousByteChannel.write](https://docs.oracle.com/javase/8/docs/api/java/nio/channels/AsynchronousByteChannel.html#write(java.nio.ByteBuffer, A, java.nio.channels.CompletionHandler<java.lang.Integer,? super A>))

