[carlie](../../../index.md) / [io.seventeenninetyone.carlie](../../index.md) / [TcpServer](../index.md) / [Connection](index.md) / [enableKeepAlive](./enable-keep-alive.md)

# enableKeepAlive

`abstract fun enableKeepAlive(initialDelay: `[`Int`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-int/index.html)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)
`abstract fun enableKeepAlive(initialDelay: `[`UInt`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-u-int/index.html)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)

Enable the TCP keep-alive functionality for the connection.

### Parameters

`initialDelay` - The initial delay before the first keep-alive probe
is sent on an idle connection.

**See Also**

[io.seventeenninetyone.carlie.TcpServer.Connection.disableKeepAlive](disable-keep-alive.md)

[io.seventeenninetyone.carlie.TcpServer.Connection.isKeepAliveEnabled](is-keep-alive-enabled.md)

