[carlie](../../index.md) / [io.seventeenninetyone.carlie](../index.md) / [TcpServer](index.md) / [listen](./listen.md)

# listen

`fun listen(): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)

Listen for connections.

If IPv6 is available, the server will accept connections on the unspecified
address \[`::`\]; otherwise, the unspecified IPv4 address \[`0.0.0.0`\]
will be used.

The server will be assigned an *arbitrary* unused port.

Once the server is listening and has been started, the address information
can be retrieved via [io.seventeenninetyone.carlie.TcpServer.address](address.md).

**Note:** While the server does indeed start listening for connections when
this call succeeds, the internal event loop that retrieves the incoming
connections doesn’t run until
[io.seventeenninetyone.carlie.TcpServer.start](start.md) is called, so this call
*must* be followed by a call to
[io.seventeenninetyone.carlie.TcpServer.start](start.md).

**See Also**

[io.seventeenninetyone.carlie.TcpServer.start](start.md)

[io.seventeenninetyone.carlie.TcpServer.address](address.md)

`fun listen(hostName: `[`String`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-string/index.html)` = ""): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)

Listen for connections.

The server will be assigned an *arbitrary* unused port.

Once the server is listening and has been started, the address information
can be retrieved via [io.seventeenninetyone.carlie.TcpServer.address](address.md).

**Note:** While the server does indeed start listening for connections when
this call succeeds, the internal event loop that retrieves the incoming
connections doesn’t run until
[io.seventeenninetyone.carlie.TcpServer.start](start.md) is called, so this call
*must* be followed by a call to
[io.seventeenninetyone.carlie.TcpServer.start](start.md).

### Parameters

`hostName` - The host name or IP address to bind on.

**See Also**

[io.seventeenninetyone.carlie.TcpServer.start](start.md)

[io.seventeenninetyone.carlie.TcpServer.address](address.md)

`fun listen(hostName: `[`String`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-string/index.html)` = "", listeningEventHandler: `[`ListeningEventHandlerFunction`](../../io.seventeenninetyone.carlie.tcp_server/-listening-event-handler-function/index.md)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)

Listen for connections.

The server will be assigned an *arbitrary* unused port.

Once the server is listening and has been started, the address information
can be retrieved via [io.seventeenninetyone.carlie.TcpServer.address](address.md).

**Note:** While the server does indeed start listening for connections when
this call succeeds, the internal event loop that retrieves the incoming
connections doesn’t run until
[io.seventeenninetyone.carlie.TcpServer.start](start.md) is called, so this call
*must* be followed by a call to
[io.seventeenninetyone.carlie.TcpServer.start](start.md).

### Parameters

`hostName` - The host name or IP address to bind on.

`listeningEventHandler` - An event handler for when the server has
started to listen.

**See Also**

[io.seventeenninetyone.carlie.TcpServer.start](start.md)

[io.seventeenninetyone.carlie.TcpServer.address](address.md)

`fun listen(listeningEventHandler: `[`ListeningEventHandlerFunction`](../../io.seventeenninetyone.carlie.tcp_server/-listening-event-handler-function/index.md)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)

Listen for connections.

If IPv6 is available, the server will accept connections on the unspecified
address \[`::`\]; otherwise, the unspecified IPv4 address \[`0.0.0.0`\]
will be used.

The server will be assigned an *arbitrary* unused port.

Once the server is listening and has been started, the address information
can be retrieved via [io.seventeenninetyone.carlie.TcpServer.address](address.md).

**Note:** While the server does indeed start listening for connections when
this call succeeds, the internal event loop that retrieves the incoming
connections doesn’t run until
[io.seventeenninetyone.carlie.TcpServer.start](start.md) is called, so this call
*must* be followed by a call to
[io.seventeenninetyone.carlie.TcpServer.start](start.md).

### Parameters

`listeningEventHandler` - An event handler for when the server has
started to listen.

**See Also**

[io.seventeenninetyone.carlie.TcpServer.start](start.md)

[io.seventeenninetyone.carlie.TcpServer.address](address.md)

`fun listen(hostName: `[`String`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-string/index.html)` = "", port: `[`UShort`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-u-short/index.html)` = 0u): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)
`fun listen(hostName: `[`String`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-string/index.html)` = "", port: `[`Int`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-int/index.html)` = 0): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)
`fun listen(hostName: `[`String`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-string/index.html)` = "", port: `[`UInt`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-u-int/index.html)` = 0u): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)

Listen for connections.

**Note:** While the server does indeed start listening for connections when
this call succeeds, the internal event loop that retrieves the incoming
connections doesn’t run until
[io.seventeenninetyone.carlie.TcpServer.start](start.md) is called, so this call
*must* be followed by a call to
[io.seventeenninetyone.carlie.TcpServer.start](start.md).

### Parameters

`hostName` - The host name or IP address to bind on.

`port` - The port to bind on.

**See Also**

[io.seventeenninetyone.carlie.TcpServer.start](start.md)

`fun listen(hostName: `[`String`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-string/index.html)` = "", port: `[`UShort`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-u-short/index.html)` = 0u, listeningEventHandler: `[`ListeningEventHandlerFunction`](../../io.seventeenninetyone.carlie.tcp_server/-listening-event-handler-function/index.md)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)
`fun listen(hostName: `[`String`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-string/index.html)` = "", port: `[`Int`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-int/index.html)` = 0, listeningEventHandler: `[`ListeningEventHandlerFunction`](../../io.seventeenninetyone.carlie.tcp_server/-listening-event-handler-function/index.md)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)
`fun listen(hostName: `[`String`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-string/index.html)` = "", port: `[`UInt`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-u-int/index.html)` = 0u, listeningEventHandler: `[`ListeningEventHandlerFunction`](../../io.seventeenninetyone.carlie.tcp_server/-listening-event-handler-function/index.md)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)

Listen for connections.

**Note:** While the server does indeed start listening for connections when
this call succeeds, the internal event loop that retrieves the incoming
connections doesn’t run until
[io.seventeenninetyone.carlie.TcpServer.start](start.md) is called, so this call
*must* be followed by a call to
[io.seventeenninetyone.carlie.TcpServer.start](start.md).

### Parameters

`hostName` - The host name or IP address to bind on.

`port` - The port to bind on.

`listeningEventHandler` - An event handler for when the server has
started to listen.

**See Also**

[io.seventeenninetyone.carlie.TcpServer.start](start.md)

`fun listen(port: `[`UShort`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-u-short/index.html)` = 0u): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)
`fun listen(port: `[`Int`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-int/index.html)` = 0): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)
`fun listen(port: `[`UInt`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-u-int/index.html)` = 0u): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)

Listen for connections.

If IPv6 is available, the server will accept connections on the unspecified
address \[`::`\]; otherwise, the unspecified IPv4 address \[`0.0.0.0`\]
will be used.

Once the server is listening and has been started, the address information
can be retrieved via [io.seventeenninetyone.carlie.TcpServer.address](address.md).

**Note:** While the server does indeed start listening for connections when
this call succeeds, the internal event loop that retrieves the incoming
connections doesn’t run until
[io.seventeenninetyone.carlie.TcpServer.start](start.md) is called, so this call
*must* be followed by a call to
[io.seventeenninetyone.carlie.TcpServer.start](start.md).

### Parameters

`port` - The port to bind on.

**See Also**

[io.seventeenninetyone.carlie.TcpServer.start](start.md)

[io.seventeenninetyone.carlie.TcpServer.address](address.md)

`fun listen(port: `[`UShort`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-u-short/index.html)` = 0u, listeningEventHandler: `[`ListeningEventHandlerFunction`](../../io.seventeenninetyone.carlie.tcp_server/-listening-event-handler-function/index.md)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)
`fun listen(port: `[`Int`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-int/index.html)` = 0, listeningEventHandler: `[`ListeningEventHandlerFunction`](../../io.seventeenninetyone.carlie.tcp_server/-listening-event-handler-function/index.md)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)
`fun listen(port: `[`UInt`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-u-int/index.html)` = 0u, listeningEventHandler: `[`ListeningEventHandlerFunction`](../../io.seventeenninetyone.carlie.tcp_server/-listening-event-handler-function/index.md)`): `[`Unit`](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin/-unit/index.html)

Listen for connections.

If IPv6 is available, the server will accept connections on the unspecified
address \[`::`\]; otherwise, the unspecified IPv4 address \[`0.0.0.0`\]
will be used.

Once the server is listening and has been started, the address information
can be retrieved via [io.seventeenninetyone.carlie.TcpServer.address](address.md).

**Note:** While the server does indeed start listening for connections when
this call succeeds, the internal event loop that retrieves the incoming
connections doesn’t run until
[io.seventeenninetyone.carlie.TcpServer.start](start.md) is called, so this call
*must* be followed by a call to
[io.seventeenninetyone.carlie.TcpServer.start](start.md).

### Parameters

`port` - The port to bind on.

`listeningEventHandler` - An event handler for when the server has
started to listen.

**See Also**

[io.seventeenninetyone.carlie.TcpServer.start](start.md)

[io.seventeenninetyone.carlie.TcpServer.address](address.md)

