# Project: Carlie

> A fast, asynchronous, and *easy-to-use* TCP server for the __JVM__ (built on [__libuv__](https://libuv.org)).

## Installation

__Note:__ __Carlie__ is built on [__libuv__](https://libuv.org), which is a C library for asynchronous I/O, so it uses the __JNI__ (__Java Native Interface__) to make everything work. Given the complexities involved with dealing with __JNI__/C code, I’ve built a cross-compilation infrastructure and [internal __JVM__ components](./src/main/kotlin/io/seventeenninetyone/carlie/utilities/NativeLibraryLoader.kt) to keep you from having to worry about any of it when just using the provided JAR, via __Maven__, __Gradle__, *etc.*

Currently, the following platforms are supported:

- __macOS__ (x86_64)
- __Linux__ (x86_64, AArch64)
- __Windows__ (x86_64, x86)

__Gradle:__

```groovy
dependencies {
  implementation 'io.seventeenninetyone:carlie:0.0.0'
}
```

__Maven:__

```xml
<dependency>
  <groupId>io.seventeenninetyone</groupId>
  <artifactId>carlie</artifactId>
  <version>0.0.0</version>
  <type>pom</type>
</dependency>
```

__JAR:__

Check out the [releases page](https://github.com/1791-labs/carlie/releases).

## Usage

Guess what? You don’t have to implement a trillion interfaces or extend even a single abstract class to use this! You don’t have to create and manage your own event loop or bootstrap anything. I wrote the [several thousands](./src/main/kotlin/io/seventeenninetyone/carlie/TcpServer.kt) [of lines](./src/main/native/carlie/tcp-server-class.c) [of code](./src/main/native/carlie/tcp-server-class.h) necessary in order to make your life easier. That’s how much I care about you and your sanity!

__Carlie__ is implemented in __Kotlin__ but makes both __Java__ and __Kotlin__ first-class citizens, as can be seen in the following examples:

__Java:__

```java
package com.example;

import io.seventeenninetyone.carlie.TcpServer;
import io.seventeenninetyone.carlie.tcp_server.ServerAlreadyListeningException;
import io.seventeenninetyone.carlie.tcp_server.ServerClosedException;
import io.seventeenninetyone.carlie.tcp_server.UvException;

import java.io.InputStream;
import java.io.OutputStream;

public class Server
{
  public static void main(String[] arguments)
    throws ServerAlreadyListeningException,
           ServerClosedException,
           UvException
  {
    final TcpServer server = new TcpServer();

    server.onClientConnected((connection) -> {
      connection.onceClosed(() -> {
        System.out.println("Client connection closed:");
        System.out.println(connection);
      });

      connection.onErrorOccurred((error) -> {
        System.out.println("Error on client connection:");
        System.out.println(error);
      });

      System.out.println("Client connected:");
      System.out.println(connection);

      // A connection implements `java.nio.channels.AsynchronousByteChannel`, so
      // you can treat it as such.

      // You can get a `java.io.InputStream` view of the connection.
      final InputStream inputStream = connection.asInputStream();
      //

      // You can get a `java.io.OutputStream` view of the connection.
      final OutputStream outputStream = connection.asOutputStream();
      //
    });

    server.listen(() -> {
      System.out.println("Server listening:");
      System.out.println(server);

      final TcpServer.Address address = server.getAddress();
      assert address != null;
      final String ipAddress = address.getIp();
      final int ipVersion = address.getVersion();
      final int addressPort = address.getPort();
      System.out.println("Server IP address: " + ipAddress);
      System.out.println("Server IP version: " + ipVersion);
      System.out.println("Server port: " + addressPort);
    });

    server.onceClosed(() -> {
      System.out.println("Server closed:");
      System.out.println(server);
    });

    server.start();
  }
}
```

__Kotlin:__

```kotlin
package com.example

import io.seventeenninetyone.carlie.TcpServer

fun main() {
  val server = TcpServer()

  server.onClientConnected {
    connection ->
      connection.onceClosed {
        println("Client connection closed:")
        println(connection)
      }

      connection.onErrorOccurred {
        error ->
          println("Error on client connection:")
          println(error)
      }

      println("Client connected:")
      println(connection)

      // A connection implements `java.nio.channels.AsynchronousByteChannel`, so
      // you can treat it as such.

      // You can get a `java.io.InputStream` view of the connection.
      val inputStream = connection.asInputStream()
      //

      // You can get a `java.io.OutputStream` view of the connection.
      val outputStream = connection.asOutputStream()
      //
  }

  server.listen {
    println("Server listening:")
    println(server)

    val (ipAddress, ipVersion, addressPort) = server.address
    println("Server IP address: ${ipAddress}")
    println("Server IP version: ${ipVersion}")
    println("Server port: ${addressPort}")
  }

  server.onceClosed {
    println("Server closed:")
    println(server)
  }

  server.start()
}
```

Check out the [API documentation](./docs/carlie/io.seventeenninetyone.carlie/-tcp-server/README.md) for more information.

## TODO

- Complete some comprehensive tests.
- Streamline the cross-compilation infrastructure.
- Set up CI infrastructure for platform testing.
- Implement TLS/SSL support.
- Implement robust HTTP server atop Carlie.
- Create some examples.

## License

```
*******************************************************************************
* Copyright 2019-present Jay B. <j@1791.io>                                   *
*                                                                             *
* Licensed under the Apache License, Version 2.0 (the "License");             *
* you may not use this file except in compliance with the License.            *
* You may obtain a copy of the License at                                     *
*                                                                             *
*     http://www.apache.org/licenses/LICENSE-2.0                              *
*                                                                             *
* Unless required by applicable law or agreed to in writing, software         *
* distributed under the License is distributed on an "AS IS" BASIS,           *
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.    *
* See the License for the specific language governing permissions and         *
* limitations under the License.                                              *
*******************************************************************************
```
