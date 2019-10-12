[carlie](../../index.md) / [io.seventeenninetyone.carlie](../index.md) / [TcpServer](index.md) / [stop](./stop.md)

# stop

`fun stop(): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)

Stop the server; *i.e.*, close all active connections and stop listening
for connections.

**Note:** Although [io.seventeenninetyone.carlie.TcpServer.start](start.md) will
attempt to clean up after itself on failures, *to be safe*, it’s
*recommended* that this be called *explicitly* in order to free up any
resources that may be held by the server.

**See Also**

[io.seventeenninetyone.carlie.TcpServer.start](start.md)

