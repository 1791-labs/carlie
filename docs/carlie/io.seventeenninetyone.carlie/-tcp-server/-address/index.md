[carlie](../../../index.md) / [io.seventeenninetyone.carlie](../../index.md) / [TcpServer](../index.md) / [Address](./index.md)

# Address

`interface Address`

An interface for internet addresses returned by a server or a server’s
connections.

**Author**
Jay B.

**See Also**

[io.seventeenninetyone.carlie.TcpServer.address](../address.md)

[io.seventeenninetyone.carlie.TcpServer.Connection.localAddress](../-connection/local-address.md)

[io.seventeenninetyone.carlie.TcpServer.Connection.remoteAddress](../-connection/remote-address.md)

### Properties

| Name | Summary |
|---|---|
| [ip](ip.md) | Get the IP address.`abstract val ip: `[`String`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-string/index.html) |
| [port](port.md) | Get the port.`abstract val port: `[`Int`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-int/index.html) |
| [version](version.md) | Get the IP address version (*i.e.*, IPv4 or IPv6).`abstract val version: `[`Int`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-int/index.html) |

### Functions

| Name | Summary |
|---|---|
| [component1](component1.md) | `abstract operator fun component1(): `[`String`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-string/index.html) |
| [component2](component2.md) | `abstract operator fun component2(): `[`Int`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-int/index.html) |
| [component3](component3.md) | `abstract operator fun component3(): `[`Int`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-int/index.html) |
