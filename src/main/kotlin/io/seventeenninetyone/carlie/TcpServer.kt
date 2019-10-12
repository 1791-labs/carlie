/**
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
 */

package io.seventeenninetyone.carlie

import com.google.common.collect.Sets
import io.seventeenninetyone.carlie.events.EventEmitter
import io.seventeenninetyone.carlie.events.event_emitter.EventHandlerFunction
import io.seventeenninetyone.carlie.tcp_server.ClientConnectedEventHandlerFunction
import io.seventeenninetyone.carlie.tcp_server.ClosedEventHandlerFunction
import io.seventeenninetyone.carlie.tcp_server.ErrorOccurredEventHandlerFunction
import io.seventeenninetyone.carlie.tcp_server.InvalidPortException
import io.seventeenninetyone.carlie.tcp_server.ListeningEventHandlerFunction
import io.seventeenninetyone.carlie.tcp_server.ServerAlreadyListeningException
import io.seventeenninetyone.carlie.tcp_server.ServerClosedException
import io.seventeenninetyone.carlie.tcp_server.UvException
import io.seventeenninetyone.carlie.utilities.NativeLibraryLoader
import io.seventeenninetyone.carlie.utilities.SimpleAtomicLock
import java.io.InputStream
import java.io.OutputStream
import java.net.Inet4Address
import java.net.Inet6Address
import java.net.InetAddress
import java.net.UnknownHostException
import java.nio.ByteBuffer
import java.nio.channels.AsynchronousByteChannel
import java.nio.channels.Channels
import java.nio.channels.ClosedChannelException
import java.nio.channels.CompletionHandler
import java.nio.channels.ReadPendingException
import java.nio.channels.WritePendingException
import java.util.UUID
import java.util.concurrent.CompletableFuture
import java.util.concurrent.Executors
import java.util.concurrent.Future
import java.util.concurrent.locks.ReentrantReadWriteLock
import kotlin.concurrent.read
import kotlin.concurrent.thread
import kotlin.concurrent.write
import kotlin.properties.ReadOnlyProperty
import kotlin.reflect.KProperty

/**
 * This class is for creating TCP servers.
 *
 * @author Jay B.
 * @see [java.lang.AutoCloseable]
 */
class TcpServer : AutoCloseable {
  companion object {
    private const val CLIENT_CONNECTED_EVENT_NAME = "CLIENT_CONNECTED"
    private const val CLIENT_NOT_CLOSEABLE_ERROR_EVENT_NAME = "CLIENT_NOT_CLOSEABLE_ERROR"
    private const val CLOSED_EVENT_NAME = "CLOSED"

    // private val connectionNativeObjectSize: Int
    //   @JvmName("_getConnectionNativeObjectSize")
    //   get() {
    //     return this.getConnectionNativeObjectSize()
    //   }
    private val connectionNativeObjectSize: Int

    private const val ERROR_OCCURRED_EVENT_NAME = "ERROR_OCCURRED"
    private const val LISTENING_EVENT_NAME = "LISTENING"

    // private val nativeObjectSize: Int
    //   @JvmName("_getNativeObjectSize")
    //   get() {
    //     return this.getNativeObjectSize()
    //   }
    private val nativeObjectSize: Int

    init {
      try {
        NativeLibraryLoader.extractAndLoad()
      } catch (error: Throwable) {
        NativeLibraryLoader.abort()
      }
      this.connectionNativeObjectSize = this.getConnectionNativeObjectSize()
      this.nativeObjectSize = this.getNativeObjectSize()
    }

    private val defaultIpv4Address by lazy {
      val ipAddressBytes = ByteArray(4)
      InetAddress.getByAddress(ipAddressBytes) as Inet4Address
    }

    private val defaultIpv6Address by lazy {
      val ipAddressBytes = ByteArray(16)
      InetAddress.getByAddress(ipAddressBytes) as Inet6Address
    }

    private val integerClass by lazy {
      java.lang.Integer::class.java
    }

    private val nullPointerExceptionClass by lazy {
      java.lang.NullPointerException::class.java
    }

    private val runtimeExceptionClass by lazy {
      java.lang.RuntimeException::class.java
    }

    private val uvExceptionClass by lazy {
      UvException::class.java
    }

    private fun createConnectionNativeObject(): ByteBuffer {
      return ByteBuffer.allocateDirect(TcpServer.connectionNativeObjectSize)
    }

    @JvmStatic
    private external fun getConnectionNativeObjectSize(): Int

    @JvmStatic
    private external fun getNativeObjectSize(): Int
  }

  private val connections: MutableSet<TcpServer.ConnectionInternal>

  /**
   * Get the number of active connections on the server.
   */
  val connectionsCount: Int
    get() {
      this.closeFlagReadWriteLock.read {
        return this.connections.size
      }
    }

  @Volatile
  private var isClosed: Boolean

  private val isClosedOrClosing: Boolean
    get() {
      return (this.isClosed ||
              this.isClosing)
    }

  @Volatile
  private var isClosing: Boolean

  /**
   * Check if the server is currently listening for connections.
   */
  @Volatile
  var isListening: Boolean
    private set

  private val nativeObject: ByteBuffer

  /**
   * Get the address of the server.
   *
   * __Note:__ Returns `null` when the server isn’t listening for connections.
   *
   * @see [io.seventeenninetyone.carlie.TcpServer.Address].
   */
  val address: TcpServer.Address? by object : ReadOnlyProperty<TcpServer, TcpServer.Address?> {
    var address: TcpServer.Address?

    init {
      this.address = null
    }

    @Synchronized
    override fun getValue(thisRef: TcpServer,
                          property: KProperty<*>): TcpServer.Address? {
      if (! thisRef.isListening) {
        return null
      }
      if (this.address != null) {
        return this.address
      }
      val createAddressMethodFunction = thisRef::createAddress
      val createAddressMethodFunctionClass = createAddressMethodFunction::class.java
      try {
        this.address = thisRef.getUvTcpBoundAddress(thisRef.nativeObject, createAddressMethodFunction, createAddressMethodFunctionClass)
      } catch (exception: UvException) {
        return null
      }
      return this.address
    }
  }

  private val closeFlagReadWriteLock by lazy {
    ReentrantReadWriteLock(true)
  }

  private val events by lazy {
    EventEmitter()
  }

  private val handleClientConnectedEventFunction by lazy {
    object : ClientConnectedEventHandlerFunction {
      override fun handle(connection: TcpServer.Connection) {
        this@TcpServer.threadPool.execute(Runnable {
          this@TcpServer.emitClientConnectedEvent(connection as TcpServer.ConnectionInternal)
        })
      }
    }
  }

  private val handleClientConnectedEventFunctionClass by lazy {
    this.handleClientConnectedEventFunction::class.java
  }

  private val handleClosedEventFunction by lazy {
    object : ClosedEventHandlerFunction {
      override fun handle() {
        this@TcpServer.threadPool.execute(Runnable {
          this@TcpServer.finishClosing()
        })
      }
    }
  }

  private val handleClosedEventFunctionClass by lazy {
    this.handleClosedEventFunction::class.java
  }

  private val handleErrorOccurredEventFunction by lazy {
    object : ErrorOccurredEventHandlerFunction {
      override fun handle(error: Throwable) {
        this@TcpServer.threadPool.execute(Runnable {
          this@TcpServer.emitErrorOccurredEvent(error)
        })
      }
    }
  }

  private val handleErrorOccurredEventFunctionClass by lazy {
    this.handleErrorOccurredEventFunction::class.java
  }

  private val operatingSystemProcessorsCount by lazy {
    val currentRuntime = Runtime.getRuntime()
    currentRuntime.availableProcessors()
  }

  private val threadPool by lazy {
    val pool = Executors.newFixedThreadPool(this.operatingSystemProcessorsCount)
    pool!!
  }

  /**
   * Create a new server.
   */
  constructor() {
    this.connections = Sets.newConcurrentHashSet()
    this.isClosed = false
    this.isClosing = false
    this.isListening = false
    this.nativeObject = ByteBuffer.allocateDirect(TcpServer.nativeObjectSize)
    val createConnectionNativeObjectStaticMethodFunction = (TcpServer)::createConnectionNativeObject
    val createConnectionNativeObjectStaticMethodFunctionClass = createConnectionNativeObjectStaticMethodFunction::class.java
    val createConnectionMethodFunction = this::createConnection
    val createConnectionMethodFunctionClass = createConnectionMethodFunction::class.java
    val nativeIsInitialized = this.initializeNative(this.nativeObject, createConnectionNativeObjectStaticMethodFunction, createConnectionNativeObjectStaticMethodFunctionClass, createConnectionMethodFunction, createConnectionMethodFunctionClass, this.handleClientConnectedEventFunction, this.handleClientConnectedEventFunctionClass, this.handleClosedEventFunction, this.handleClosedEventFunctionClass, this.handleErrorOccurredEventFunction, this.handleErrorOccurredEventFunctionClass, TcpServer.integerClass, TcpServer.nullPointerExceptionClass, TcpServer.runtimeExceptionClass, TcpServer.uvExceptionClass)
    if (! nativeIsInitialized) {
      throw RuntimeException()
    }
  }

  private external fun initializeNative(nativeObject: ByteBuffer,
                                        createConnectionNativeObjectStaticMethodFunction: Function0<ByteBuffer>,
                                        createConnectionNativeObjectStaticMethodFunctionClass: Class<out Function0<ByteBuffer>>,
                                        createConnectionMethodFunction: Function1<ByteBuffer, TcpServer.ConnectionInternal>,
                                        createConnectionMethodFunctionClass: Class<out Function1<ByteBuffer, TcpServer.ConnectionInternal>>,
                                        handleClientConnectedEventFunction: ClientConnectedEventHandlerFunction,
                                        handleClientConnectedEventFunctionClass: Class<out ClientConnectedEventHandlerFunction>,
                                        handleClosedEventFunction: ClosedEventHandlerFunction,
                                        handleClosedEventFunctionClass: Class<out ClosedEventHandlerFunction>,
                                        handleErrorOccurredEventFunction: ErrorOccurredEventHandlerFunction,
                                        handleErrorOccurredEventFunctionClass: Class<out ErrorOccurredEventHandlerFunction>,
                                        integerClass: Class<java.lang.Integer>,
                                        nullPointerExceptionClass: Class<java.lang.NullPointerException>,
                                        runtimeExceptionClass: Class<java.lang.RuntimeException>,
                                        uvExceptionClass: Class<UvException>): Boolean

  /**
   * @suppress
   */
  override fun close() {
    if (this.isClosedOrClosing) return
    this.closeFlagReadWriteLock.write {
      this.closeConnections()
      this.closeUvTcpHandle(this.nativeObject)
      this.isListening = false
      this.isClosing = true
    }
  }

  private external fun closeNative(nativeObject: ByteBuffer)

  /**
   * @suppress
   */
  protected fun finalize() {
    this.close()
  }

  private fun addConnection(connection: TcpServer.ConnectionInternal) {
    this.closeFlagReadWriteLock.read {
      this.connections.add(connection)
    }
  }

  @Throws(UvException::class)
  private external fun bindUvTcpHandle(nativeObject: ByteBuffer,
                                       ipAddress: String,
                                       ipAddressType: Byte,
                                       port: Int)

  private fun closeConnections() {
    this.closeConnections(this.connections)
  }

  private fun closeConnections(connections: MutableSet<TcpServer.ConnectionInternal>) {
    val connectionsNotClosed: MutableSet<TcpServer.ConnectionInternal> = Sets.newLinkedHashSet()
    connections.forEach l@{
      connection ->
        if (! connection.isCloseable) {
          connectionsNotClosed.add(connection)
          return@l
        }
        connection.close()
    }
    connections.clear()
    if (connectionsNotClosed.isNotEmpty()) {
      this.closeConnections(connectionsNotClosed)
    }
  }

  private external fun closeUvTcpHandle(nativeObject: ByteBuffer)

  private fun createAddress(ip: String,
                            version: Int,
                            port: Int): TcpServer.AddressInternal {
    return TcpServer.AddressInternal(ip, version, port)
  }

  private fun createConnection(nativeObject: ByteBuffer): TcpServer.ConnectionInternal {
    return this.ConnectionInternal(nativeObject)
  }

  private fun emitClientConnectedEvent(connection: TcpServer.ConnectionInternal) {
    this.events.emitEvent(TcpServer.CLIENT_CONNECTED_EVENT_NAME, connection)
  }

  private fun emitClosedEvent() {
    this.events.emitEvent(TcpServer.CLOSED_EVENT_NAME)
  }

  private fun emitErrorOccurredEvent(error: Throwable) {
    this.events.emitEvent(TcpServer.ERROR_OCCURRED_EVENT_NAME, error)
  }

  private fun emitListeningEvent() {
    this.events.emitEvent(TcpServer.LISTENING_EVENT_NAME)
  }

  private fun finishClosing() {
    if (this.isClosed) return
    if (! this.isClosing) return
    this.closeFlagReadWriteLock.write {
      this.closeNative(this.nativeObject)
      // // NOTE: See the note in `this.start()`.
      // this.nativeObject.clear()
      this.isClosing = false
      this.isClosed = true
      this.emitClosedEvent()
      this.events.removeAllEventHandlers()
    }
  }

  @Throws(UvException::class)
  private external fun getUvTcpBoundAddress(nativeObject: ByteBuffer,
                                            createAddressMethodFunction: Function3<String, Int, Int, TcpServer.AddressInternal>,
                                            createAddressMethodFunctionClass: Class<out Function3<String, Int, Int, TcpServer.AddressInternal>>): TcpServer.AddressInternal

  @Throws(UvException::class)
  private external fun initializeUvTcpHandle(nativeObject: ByteBuffer)

  /**
   * Listen for connections.
   *
   * If IPv6 is available, the server will accept connections on the unspecified
   * address \[`::`\]; otherwise, the unspecified IPv4 address \[`0.0.0.0`\]
   * will be used.
   *
   * The server will be assigned an *arbitrary* unused port.
   *
   * Once the server is listening and has been started, the address information
   * can be retrieved via [io.seventeenninetyone.carlie.TcpServer.address].
   *
   * __Note:__ While the server does indeed start listening for connections when
   * this call succeeds, the internal event loop that retrieves the incoming
   * connections doesn’t run until
   * [io.seventeenninetyone.carlie.TcpServer.start] is called, so this call
   * *must* be followed by a call to
   * [io.seventeenninetyone.carlie.TcpServer.start].
   *
   * @see [io.seventeenninetyone.carlie.TcpServer.start]
   * @see [io.seventeenninetyone.carlie.TcpServer.address]
   */
  @Throws(ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UvException::class)
  fun listen() {
    return this.listen(0u.toUShort())
  }

  /**
   * Listen for connections.
   *
   * The server will be assigned an *arbitrary* unused port.
   *
   * Once the server is listening and has been started, the address information
   * can be retrieved via [io.seventeenninetyone.carlie.TcpServer.address].
   *
   * __Note:__ While the server does indeed start listening for connections when
   * this call succeeds, the internal event loop that retrieves the incoming
   * connections doesn’t run until
   * [io.seventeenninetyone.carlie.TcpServer.start] is called, so this call
   * *must* be followed by a call to
   * [io.seventeenninetyone.carlie.TcpServer.start].
   *
   * @param hostName The host name or IP address to bind on.
   * @see [io.seventeenninetyone.carlie.TcpServer.start]
   * @see [io.seventeenninetyone.carlie.TcpServer.address]
   */
  @Throws(ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UnknownHostException::class,
          UvException::class)
  fun listen(hostName: String = "") {
    return this.listen(hostName, 0u.toUShort())
  }

  @Throws(ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UnknownHostException::class,
          UvException::class)
  @JvmName("listen1")
  private fun listen(hostName: String = "",
                     listeningEventHandler: ListeningEventHandlerFunction?) {
    try {
      return this.listen(hostName, 0u.toUShort(), listeningEventHandler)
    } catch (error: Throwable) {
      thread {
        this.close()
      }
      throw error
    }
  }

  /**
   * Listen for connections.
   *
   * The server will be assigned an *arbitrary* unused port.
   *
   * Once the server is listening and has been started, the address information
   * can be retrieved via [io.seventeenninetyone.carlie.TcpServer.address].
   *
   * __Note:__ While the server does indeed start listening for connections when
   * this call succeeds, the internal event loop that retrieves the incoming
   * connections doesn’t run until
   * [io.seventeenninetyone.carlie.TcpServer.start] is called, so this call
   * *must* be followed by a call to
   * [io.seventeenninetyone.carlie.TcpServer.start].
   *
   * @param hostName The host name or IP address to bind on.
   * @param listeningEventHandler An event handler for when the server has
   * started to listen.
   * @see [io.seventeenninetyone.carlie.TcpServer.start]
   * @see [io.seventeenninetyone.carlie.TcpServer.address]
   */
  @Throws(ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UnknownHostException::class,
          UvException::class)
  fun listen(hostName: String = "",
             listeningEventHandler: ListeningEventHandlerFunction) {
    return this.listen(hostName, listeningEventHandler as ListeningEventHandlerFunction?)
  }

  /**
   * @suppress
   */
  @Throws(ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UnknownHostException::class,
          UvException::class)
  @JvmSynthetic
  fun listen(hostName: String = "",
             listeningEventHandler: () -> Unit) {
    return this.listen(hostName, object : ListeningEventHandlerFunction {
      override fun handle() {
        return listeningEventHandler()
      }
    })
  }

  /**
   * Listen for connections.
   *
   * If IPv6 is available, the server will accept connections on the unspecified
   * address \[`::`\]; otherwise, the unspecified IPv4 address \[`0.0.0.0`\]
   * will be used.
   *
   * The server will be assigned an *arbitrary* unused port.
   *
   * Once the server is listening and has been started, the address information
   * can be retrieved via [io.seventeenninetyone.carlie.TcpServer.address].
   *
   * __Note:__ While the server does indeed start listening for connections when
   * this call succeeds, the internal event loop that retrieves the incoming
   * connections doesn’t run until
   * [io.seventeenninetyone.carlie.TcpServer.start] is called, so this call
   * *must* be followed by a call to
   * [io.seventeenninetyone.carlie.TcpServer.start].
   *
   * @param listeningEventHandler An event handler for when the server has
   * started to listen.
   * @see [io.seventeenninetyone.carlie.TcpServer.start]
   * @see [io.seventeenninetyone.carlie.TcpServer.address]
   */
  @Throws(ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UvException::class)
  fun listen(listeningEventHandler: ListeningEventHandlerFunction) {
    return this.listen(0u.toUShort(), listeningEventHandler)
  }

  /**
   * @suppress
   */
  @Throws(ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UvException::class)
  @JvmSynthetic
  fun listen(listeningEventHandler: () -> Unit) {
    return this.listen(object : ListeningEventHandlerFunction {
      override fun handle() {
        return listeningEventHandler()
      }
    })
  }

  /**
   * Listen for connections.
   *
   * __Note:__ While the server does indeed start listening for connections when
   * this call succeeds, the internal event loop that retrieves the incoming
   * connections doesn’t run until
   * [io.seventeenninetyone.carlie.TcpServer.start] is called, so this call
   * *must* be followed by a call to
   * [io.seventeenninetyone.carlie.TcpServer.start].
   *
   * @param hostName The host name or IP address to bind on.
   * @param port The port to bind on.
   * @see [io.seventeenninetyone.carlie.TcpServer.start]
   */
  @Throws(ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UnknownHostException::class,
          UvException::class)
  @JvmSynthetic
  fun listen(hostName: String = "",
             port: UShort = 0u) {
    try {
      return this.listen(hostName, port, null as ListeningEventHandlerFunction?)
    } catch (error: Throwable) {
      thread {
        this.close()
      }
      throw error
    }
  }

  /**
   * Listen for connections.
   *
   * __Note:__ While the server does indeed start listening for connections when
   * this call succeeds, the internal event loop that retrieves the incoming
   * connections doesn’t run until
   * [io.seventeenninetyone.carlie.TcpServer.start] is called, so this call
   * *must* be followed by a call to
   * [io.seventeenninetyone.carlie.TcpServer.start].
   *
   * @param hostName The host name or IP address to bind on.
   * @param port The port to bind on.
   * @see [io.seventeenninetyone.carlie.TcpServer.start]
   */
  @Throws(InvalidPortException::class,
          ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UnknownHostException::class,
          UvException::class)
  fun listen(hostName: String = "",
             port: Int = 0) {
    return this.listen(hostName, port, null as ListeningEventHandlerFunction?)
  }

  /**
   * Listen for connections.
   *
   * __Note:__ While the server does indeed start listening for connections when
   * this call succeeds, the internal event loop that retrieves the incoming
   * connections doesn’t run until
   * [io.seventeenninetyone.carlie.TcpServer.start] is called, so this call
   * *must* be followed by a call to
   * [io.seventeenninetyone.carlie.TcpServer.start].
   *
   * @param hostName The host name or IP address to bind on.
   * @param port The port to bind on.
   * @see [io.seventeenninetyone.carlie.TcpServer.start]
   */
  @Throws(InvalidPortException::class,
          ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UnknownHostException::class,
          UvException::class)
  @JvmSynthetic
  fun listen(hostName: String = "",
             port: UInt = 0u) {
    return this.listen(hostName, port, null as ListeningEventHandlerFunction?)
  }

  @Throws(ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UnknownHostException::class,
          UvException::class)
  @Synchronized
  private fun listen(hostName: String = "",
                     port: UShort = 0u,
                     listeningEventHandler: ListeningEventHandlerFunction?) {
    if (this.isClosedOrClosing) {
      throw ServerClosedException()
    }
    if (this.isListening) {
      throw ServerAlreadyListeningException()
    }
    val hostName1 = hostName.trim()
    if (hostName1 == "") {
      return this.listen(TcpServer.defaultIpv6Address, port, listeningEventHandler)
    }
    val ipAddress: InetAddress
    try {
      ipAddress = InetAddress.getByName(hostName)
    } catch (exception: Exception) {
      when (exception) {
        is UnknownHostException -> throw exception
        else -> throw exception
      }
    }
    when (ipAddress) {
      is Inet6Address -> {
        return this.listen(ipAddress, port, listeningEventHandler)
      }
      is Inet4Address -> {
        return this.listen(ipAddress, port, listeningEventHandler)
      }
    }
  }

  /**
   * Listen for connections.
   *
   * __Note:__ While the server does indeed start listening for connections when
   * this call succeeds, the internal event loop that retrieves the incoming
   * connections doesn’t run until
   * [io.seventeenninetyone.carlie.TcpServer.start] is called, so this call
   * *must* be followed by a call to
   * [io.seventeenninetyone.carlie.TcpServer.start].
   *
   * @param hostName The host name or IP address to bind on.
   * @param port The port to bind on.
   * @param listeningEventHandler An event handler for when the server has
   * started to listen.
   * @see [io.seventeenninetyone.carlie.TcpServer.start]
   */
  @Throws(ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UnknownHostException::class,
          UvException::class)
  @JvmSynthetic
  fun listen(hostName: String = "",
             port: UShort = 0u,
             listeningEventHandler: ListeningEventHandlerFunction) {
    return this.listen(hostName, port, listeningEventHandler as ListeningEventHandlerFunction?)
  }

  /**
   * @suppress
   */
  @Throws(ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UnknownHostException::class,
          UvException::class)
  @JvmSynthetic
  fun listen(hostName: String = "",
             port: UShort = 0u,
             listeningEventHandler: () -> Unit) {
    return this.listen(hostName, port, object : ListeningEventHandlerFunction {
      override fun handle() {
        return listeningEventHandler()
      }
    })
  }

  @Throws(InvalidPortException::class,
          ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UnknownHostException::class,
          UvException::class)
  @JvmName("listen1")
  private fun listen(hostName: String = "",
                     port: Int = 0,
                     listeningEventHandler: ListeningEventHandlerFunction?) {
    try {
      this.validatePort(port)
      return this.listen(hostName, port.toUShort(), listeningEventHandler)
    } catch (error: Throwable) {
      thread {
        this.close()
      }
      throw error
    }
  }

  /**
   * Listen for connections.
   *
   * __Note:__ While the server does indeed start listening for connections when
   * this call succeeds, the internal event loop that retrieves the incoming
   * connections doesn’t run until
   * [io.seventeenninetyone.carlie.TcpServer.start] is called, so this call
   * *must* be followed by a call to
   * [io.seventeenninetyone.carlie.TcpServer.start].
   *
   * @param hostName The host name or IP address to bind on.
   * @param port The port to bind on.
   * @param listeningEventHandler An event handler for when the server has
   * started to listen.
   * @see [io.seventeenninetyone.carlie.TcpServer.start]
   */
  @Throws(InvalidPortException::class,
          ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UnknownHostException::class,
          UvException::class)
  fun listen(hostName: String = "",
             port: Int = 0,
             listeningEventHandler: ListeningEventHandlerFunction) {
    return this.listen(hostName, port, listeningEventHandler as ListeningEventHandlerFunction?)
  }

  /**
   * @suppress
   */
  @Throws(InvalidPortException::class,
          ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UnknownHostException::class,
          UvException::class)
  @JvmSynthetic
  fun listen(hostName: String = "",
             port: Int = 0,
             listeningEventHandler: () -> Unit) {
    return this.listen(hostName, port, object : ListeningEventHandlerFunction {
      override fun handle() {
        return listeningEventHandler()
      }
    })
  }

  @Throws(InvalidPortException::class,
          ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UnknownHostException::class,
          UvException::class)
  private fun listen(hostName: String = "",
                     port: UInt = 0u,
                     listeningEventHandler: ListeningEventHandlerFunction?) {
    try {
      this.validatePort(port)
      return this.listen(hostName, port.toUShort(), listeningEventHandler)
    } catch (error: Throwable) {
      thread {
        this.close()
      }
      throw error
    }
  }

  /**
   * Listen for connections.
   *
   * __Note:__ While the server does indeed start listening for connections when
   * this call succeeds, the internal event loop that retrieves the incoming
   * connections doesn’t run until
   * [io.seventeenninetyone.carlie.TcpServer.start] is called, so this call
   * *must* be followed by a call to
   * [io.seventeenninetyone.carlie.TcpServer.start].
   *
   * @param hostName The host name or IP address to bind on.
   * @param port The port to bind on.
   * @param listeningEventHandler An event handler for when the server has
   * started to listen.
   * @see [io.seventeenninetyone.carlie.TcpServer.start]
   */
  @Throws(InvalidPortException::class,
          ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UnknownHostException::class,
          UvException::class)
  @JvmSynthetic
  fun listen(hostName: String = "",
             port: UInt = 0u,
             listeningEventHandler: ListeningEventHandlerFunction) {
    return this.listen(hostName, port, listeningEventHandler as ListeningEventHandlerFunction?)
  }

  /**
   * @suppress
   */
  @Throws(InvalidPortException::class,
          ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UnknownHostException::class,
          UvException::class)
  @JvmSynthetic
  fun listen(hostName: String = "",
             port: UInt = 0u,
             listeningEventHandler: () -> Unit) {
    return this.listen(hostName, port, object : ListeningEventHandlerFunction {
      override fun handle() {
        return listeningEventHandler()
      }
    })
  }

  /**
   * Listen for connections.
   *
   * If IPv6 is available, the server will accept connections on the unspecified
   * address \[`::`\]; otherwise, the unspecified IPv4 address \[`0.0.0.0`\]
   * will be used.
   *
   * Once the server is listening and has been started, the address information
   * can be retrieved via [io.seventeenninetyone.carlie.TcpServer.address].
   *
   * __Note:__ While the server does indeed start listening for connections when
   * this call succeeds, the internal event loop that retrieves the incoming
   * connections doesn’t run until
   * [io.seventeenninetyone.carlie.TcpServer.start] is called, so this call
   * *must* be followed by a call to
   * [io.seventeenninetyone.carlie.TcpServer.start].
   *
   * @param port The port to bind on.
   * @see [io.seventeenninetyone.carlie.TcpServer.start]
   * @see [io.seventeenninetyone.carlie.TcpServer.address]
   */
  @Throws(ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UvException::class)
  @JvmSynthetic
  fun listen(port: UShort = 0u) {
    return this.listen("", port)
  }

  /**
   * Listen for connections.
   *
   * If IPv6 is available, the server will accept connections on the unspecified
   * address \[`::`\]; otherwise, the unspecified IPv4 address \[`0.0.0.0`\]
   * will be used.
   *
   * Once the server is listening and has been started, the address information
   * can be retrieved via [io.seventeenninetyone.carlie.TcpServer.address].
   *
   * __Note:__ While the server does indeed start listening for connections when
   * this call succeeds, the internal event loop that retrieves the incoming
   * connections doesn’t run until
   * [io.seventeenninetyone.carlie.TcpServer.start] is called, so this call
   * *must* be followed by a call to
   * [io.seventeenninetyone.carlie.TcpServer.start].
   *
   * @param port The port to bind on.
   * @see [io.seventeenninetyone.carlie.TcpServer.start]
   * @see [io.seventeenninetyone.carlie.TcpServer.address]
   */
  @Throws(InvalidPortException::class,
          ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UvException::class)
  fun listen(port: Int = 0) {
    return this.listen("", port)
  }

  /**
   * Listen for connections.
   *
   * If IPv6 is available, the server will accept connections on the unspecified
   * address \[`::`\]; otherwise, the unspecified IPv4 address \[`0.0.0.0`\]
   * will be used.
   *
   * Once the server is listening and has been started, the address information
   * can be retrieved via [io.seventeenninetyone.carlie.TcpServer.address].
   *
   * __Note:__ While the server does indeed start listening for connections when
   * this call succeeds, the internal event loop that retrieves the incoming
   * connections doesn’t run until
   * [io.seventeenninetyone.carlie.TcpServer.start] is called, so this call
   * *must* be followed by a call to
   * [io.seventeenninetyone.carlie.TcpServer.start].
   *
   * @param port The port to bind on.
   * @see [io.seventeenninetyone.carlie.TcpServer.start]
   * @see [io.seventeenninetyone.carlie.TcpServer.address]
   */
  @Throws(InvalidPortException::class,
          ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UvException::class)
  @JvmSynthetic
  fun listen(port: UInt = 0u) {
    return this.listen("", port)
  }

  @Throws(ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UvException::class)
  private fun listen(port: UShort = 0u,
                     listeningEventHandler: ListeningEventHandlerFunction?) {
    try {
      return this.listen("", port, listeningEventHandler)
    } catch (error: Throwable) {
      thread {
        this.close()
      }
      throw error
    }
  }

  /**
   * Listen for connections.
   *
   * If IPv6 is available, the server will accept connections on the unspecified
   * address \[`::`\]; otherwise, the unspecified IPv4 address \[`0.0.0.0`\]
   * will be used.
   *
   * Once the server is listening and has been started, the address information
   * can be retrieved via [io.seventeenninetyone.carlie.TcpServer.address].
   *
   * __Note:__ While the server does indeed start listening for connections when
   * this call succeeds, the internal event loop that retrieves the incoming
   * connections doesn’t run until
   * [io.seventeenninetyone.carlie.TcpServer.start] is called, so this call
   * *must* be followed by a call to
   * [io.seventeenninetyone.carlie.TcpServer.start].
   *
   * @param port The port to bind on.
   * @param listeningEventHandler An event handler for when the server has
   * started to listen.
   * @see [io.seventeenninetyone.carlie.TcpServer.start]
   * @see [io.seventeenninetyone.carlie.TcpServer.address]
   */
  @Throws(ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UvException::class)
  @JvmSynthetic
  fun listen(port: UShort = 0u,
             listeningEventHandler: ListeningEventHandlerFunction) {
    return this.listen(port, listeningEventHandler as ListeningEventHandlerFunction?)
  }

  /**
   * @suppress
   */
  @Throws(ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UvException::class)
  @JvmSynthetic
  fun listen(port: UShort = 0u,
             listeningEventHandler: () -> Unit) {
    return this.listen(port, object : ListeningEventHandlerFunction {
      override fun handle() {
        return listeningEventHandler()
      }
    })
  }

  @Throws(InvalidPortException::class,
          ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UvException::class)
  @JvmName("listen1")
  private fun listen(port: Int = 0,
                     listeningEventHandler: ListeningEventHandlerFunction?) {
    return this.listen("", port, listeningEventHandler)
  }

  /**
   * Listen for connections.
   *
   * If IPv6 is available, the server will accept connections on the unspecified
   * address \[`::`\]; otherwise, the unspecified IPv4 address \[`0.0.0.0`\]
   * will be used.
   *
   * Once the server is listening and has been started, the address information
   * can be retrieved via [io.seventeenninetyone.carlie.TcpServer.address].
   *
   * __Note:__ While the server does indeed start listening for connections when
   * this call succeeds, the internal event loop that retrieves the incoming
   * connections doesn’t run until
   * [io.seventeenninetyone.carlie.TcpServer.start] is called, so this call
   * *must* be followed by a call to
   * [io.seventeenninetyone.carlie.TcpServer.start].
   *
   * @param port The port to bind on.
   * @param listeningEventHandler An event handler for when the server has
   * started to listen.
   * @see [io.seventeenninetyone.carlie.TcpServer.start]
   * @see [io.seventeenninetyone.carlie.TcpServer.address]
   */
  @Throws(InvalidPortException::class,
          ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UvException::class)
  fun listen(port: Int = 0,
             listeningEventHandler: ListeningEventHandlerFunction) {
    return this.listen(port, listeningEventHandler as ListeningEventHandlerFunction?)
  }

  /**
   * @suppress
   */
  @Throws(InvalidPortException::class,
          ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UvException::class)
  @JvmSynthetic
  fun listen(port: Int = 0,
             listeningEventHandler: () -> Unit) {
    return this.listen(port, object : ListeningEventHandlerFunction {
      override fun handle() {
        return listeningEventHandler()
      }
    })
  }

  @Throws(InvalidPortException::class,
          ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UvException::class)
  private fun listen(port: UInt = 0u,
                     listeningEventHandler: ListeningEventHandlerFunction?) {
    return this.listen("", port, listeningEventHandler)
  }

  /**
   * Listen for connections.
   *
   * If IPv6 is available, the server will accept connections on the unspecified
   * address \[`::`\]; otherwise, the unspecified IPv4 address \[`0.0.0.0`\]
   * will be used.
   *
   * Once the server is listening and has been started, the address information
   * can be retrieved via [io.seventeenninetyone.carlie.TcpServer.address].
   *
   * __Note:__ While the server does indeed start listening for connections when
   * this call succeeds, the internal event loop that retrieves the incoming
   * connections doesn’t run until
   * [io.seventeenninetyone.carlie.TcpServer.start] is called, so this call
   * *must* be followed by a call to
   * [io.seventeenninetyone.carlie.TcpServer.start].
   *
   * @param port The port to bind on.
   * @param listeningEventHandler An event handler for when the server has
   * started to listen.
   * @see [io.seventeenninetyone.carlie.TcpServer.start]
   * @see [io.seventeenninetyone.carlie.TcpServer.address]
   */
  @Throws(InvalidPortException::class,
          ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UvException::class)
  @JvmSynthetic
  fun listen(port: UInt = 0u,
             listeningEventHandler: ListeningEventHandlerFunction) {
    return this.listen(port, listeningEventHandler as ListeningEventHandlerFunction?)
  }

  /**
   * @suppress
   */
  @Throws(InvalidPortException::class,
          ServerAlreadyListeningException::class,
          ServerClosedException::class,
          UvException::class)
  @JvmSynthetic
  fun listen(port: UInt = 0u,
             listeningEventHandler: () -> Unit) {
    return this.listen(port, object : ListeningEventHandlerFunction {
      override fun handle() {
        return listeningEventHandler()
      }
    })
  }

  @Throws(UvException::class)
  private fun listen(ipAddress: Inet6Address,
                     port: UShort,
                     listeningEventHandler: ListeningEventHandlerFunction?) {
    return this.listen(ipAddress, 6u.toUByte(), port, listeningEventHandler)
  }

  @Throws(UvException::class)
  private fun listen(ipAddress: Inet4Address,
                     port: UShort,
                     listeningEventHandler: ListeningEventHandlerFunction?) {
    return this.listen(ipAddress, 4u.toUByte(), port, listeningEventHandler)
  }

  @Throws(UvException::class)
  private fun listen(ipAddress: InetAddress,
                     ipAddressType: UByte,
                     port: UShort,
                     listeningEventHandler: ListeningEventHandlerFunction?) {
    this.initializeUvTcpHandle(this.nativeObject)
    try {
      this.bindUvTcpHandle(this.nativeObject, ipAddress.hostAddress, ipAddressType.toByte(), port.toInt())
    } catch (exception: UvException) {
      if (ipAddress == TcpServer.defaultIpv6Address) {
        return this.listen(TcpServer.defaultIpv4Address, 4u.toUByte(), port, listeningEventHandler)
      }
      throw exception
    }
    this.uvTcpListen(nativeObject)
    this.isListening = true
    if (listeningEventHandler == null) return
    this.onceListening(listeningEventHandler)
  }

  /**
   * Attach an event handler for when the server has closed.
   *
   * @param handler The event handler.
   * @see [io.seventeenninetyone.carlie.tcp_server.ClosedEventHandlerFunction]
   */
  fun onceClosed(handler: ClosedEventHandlerFunction) {
    return this.onceClosedEvent(handler)
  }

  /**
   * @suppress
   */
  @JvmSynthetic
  fun onceClosed(handler: () -> Unit) {
    return this.onceClosed(object : ClosedEventHandlerFunction {
      override fun handle() {
        return handler()
      }
    })
  }

  private fun onceClosedEvent(handler: ClosedEventHandlerFunction) {
    if (this.isClosed) return
    this.events.onceEvent(TcpServer.CLOSED_EVENT_NAME, object : EventHandlerFunction {
      override fun handle(data: Any?) {
        handler()
      }
    })
  }

  /**
   * Attach an event handler for when the server has started to listen.
   *
   * @param handler The event handler.
   * @see [io.seventeenninetyone.carlie.tcp_server.ListeningEventHandlerFunction]
   */
  fun onceListening(handler: ListeningEventHandlerFunction) {
    return this.onceListeningEvent(handler)
  }

  /**
   * @suppress
   */
  @JvmSynthetic
  fun onceListening(handler: () -> Unit) {
    return this.onceListening(object : ListeningEventHandlerFunction {
      override fun handle() {
        return handler()
      }
    })
  }

  private fun onceListeningEvent(handler: ListeningEventHandlerFunction) {
    if (this.isClosedOrClosing) return
    this.events.onceEvent(TcpServer.LISTENING_EVENT_NAME, object : EventHandlerFunction {
      override fun handle(data: Any?) {
        handler()
      }
    })
  }

  /**
   * Attach an event handler for incoming connections on the server.
   *
   * @param handler The event handler.
   * @see [io.seventeenninetyone.carlie.tcp_server.ClientConnectedEventHandlerFunction]
   */
  fun onClientConnected(handler: ClientConnectedEventHandlerFunction) {
    return this.onClientConnectedEvent(handler)
  }

  /**
   * @suppress
   */
  @JvmSynthetic
  fun onClientConnected(handler: (@ParameterName("connection") TcpServer.Connection) -> Unit) {
    return this.onClientConnected(object : ClientConnectedEventHandlerFunction {
      override fun handle(connection: TcpServer.Connection) {
        return handler(connection)
      }
    })
  }

  private fun onClientConnectedEvent(handler: ClientConnectedEventHandlerFunction) {
    if (this.isClosedOrClosing) return
    this.events.onEvent(TcpServer.CLIENT_CONNECTED_EVENT_NAME, object : EventHandlerFunction {
      override fun handle(data: Any?) {
        handler(data as TcpServer.Connection)
      }
    })
  }

  /**
   * Attach an event handler for errors occurring on the server.
   *
   * @param handler The event handler.
   * @see [io.seventeenninetyone.carlie.tcp_server.ErrorOccurredEventHandlerFunction]
   */
  fun onErrorOccurred(handler: ErrorOccurredEventHandlerFunction) {
    return this.onErrorOccurredEvent(handler)
  }

  /**
   * @suppress
   */
  @JvmSynthetic
  fun onErrorOccurred(handler: (@ParameterName("error") Throwable) -> Unit) {
    return this.onErrorOccurred(object : ErrorOccurredEventHandlerFunction {
      override fun handle(error: Throwable) {
        return handler(error)
      }
    })
  }

  private fun onErrorOccurredEvent(handler: ErrorOccurredEventHandlerFunction) {
    if (this.isClosedOrClosing) return
    this.events.onEvent(TcpServer.ERROR_OCCURRED_EVENT_NAME, object : EventHandlerFunction {
      override fun handle(data: Any?) {
        handler(data as Throwable)
      }
    })
  }

  private fun removeConnection(connection: TcpServer.ConnectionInternal) {
    this.closeFlagReadWriteLock.read {
      this.connections.remove(connection)
    }
  }

  /**
   * Start the server.
   *
   * __Note:__ The server *must* already be listening, so this call *must* be
   * preceeded by a call to [io.seventeenninetyone.carlie.TcpServer.listen].
   *
   * @see [io.seventeenninetyone.carlie.TcpServer.listen]
   */
  fun start() {
    synchronized(this) {
      if (! this.isListening) return
    }
    thread(priority = Thread.MAX_PRIORITY) {
      this.use {
        this.uvRun()
        // NOTE: This being done here rather than in `this.finishClosing()`
        // because this is when it’s guaranteed that the native object is no
        // longer being manipulated in the native layer.
        this.nativeObject.clear()
      }
    }
    this.emitListeningEvent()
  }

  /**
   * Stop the server; *i.e.*, close all active connections and stop listening
   * for connections.
   *
   * __Note:__ Although [io.seventeenninetyone.carlie.TcpServer.start] will
   * attempt to clean up after itself on failures, *to be safe*, it’s
   * *recommended* that this be called *explicitly* in order to free up any
   * resources that may be held by the server.
   *
   * @see [io.seventeenninetyone.carlie.TcpServer.start]
   */
  fun stop() {
    this.close()
  }

  /**
   * Produce a string representation of the server and its state.
   */
  override fun toString(): String {
    val prefix = "TCP server {"
    val suffix = "}"
    return when {
      this.isClosing -> "${prefix}status=CLOSING${suffix}"
      this.isClosed -> "${prefix}status=CLOSED${suffix}"
      (! this.isListening) -> "${prefix}status=NOT_STARTED${suffix}"
      else -> {
        val (ipAddress, addressVersion, addressPort) = this.address!!
        val address = "(IP=${ipAddress}, version=${addressVersion}, port=${addressPort})"
        val spacePrefix = {
          val spaces = CharArray(prefix.length) {
            ' '
          }
          spaces.joinToString("")
        }()
        var result = ""
        result += prefix
        result += "status=LISTENING"
        result += ",\n"
        result += spacePrefix
        result += "address=${address}"
        result += suffix
        result
      }
    }
  }

  @Throws(UvException::class)
  @Synchronized
  private fun uvRun() {
    return this.uvRun(this.nativeObject)
  }

  @Throws(UvException::class)
  private external fun uvRun(nativeObject: ByteBuffer)

  @Throws(UvException::class)
  private external fun uvTcpListen(nativeObject: ByteBuffer)

  @Throws(InvalidPortException::class)
  private fun validatePort(port: Int) {
    if ((port < 0) ||
        (port > UShort.MAX_VALUE.toInt())) {
      throw InvalidPortException()
    }
  }

  @Throws(InvalidPortException::class)
  private fun validatePort(port: UInt) {
    if (port > UShort.MAX_VALUE.toUInt()) {
      throw InvalidPortException()
    }
  }

  /**
 * An interface for internet addresses returned by a server or a server’s
 * connections.
 *
 * @author Jay B.
 * @see [io.seventeenninetyone.carlie.TcpServer.address]
 * @see [io.seventeenninetyone.carlie.TcpServer.Connection.localAddress]
 * @see [io.seventeenninetyone.carlie.TcpServer.Connection.remoteAddress]
 */
  interface Address {
    /**
     * Get the IP address.
     */
    val ip: String
    /**
     * Get the port.
     */
    val port: Int
    /**
     * Get the IP address version (*i.e.*, IPv4 or IPv6).
     */
    val version: Int

    @JvmSynthetic
    operator fun component1(): String

    @JvmSynthetic
    operator fun component2(): Int

    @JvmSynthetic
    operator fun component3(): Int

    /**
     * @suppress
     */
    override fun toString(): String
  }

  private data class AddressInternal(override val ip: String,
                                     override val version: Int,
                                     override val port: Int) : TcpServer.Address

  /**
   * An interface for a server’s connections.
   *
   * @author Jay B.
   * @see [io.seventeenninetyone.carlie.TcpServer.onClientConnected]
   * @see [io.seventeenninetyone.carlie.tcp_server.ClientConnectedEventHandlerFunction]
   * @see [java.nio.channels.AsynchronousByteChannel]
   * @see [java.lang.AutoCloseable]
   */
  interface Connection : AsynchronousByteChannel,
                         AutoCloseable {
    /**
     * Get the unique identifier of the connection.
     */
    val id: UUID
    /**
     * Check if the TCP keep-alive functionality is enabled for the connection.
     *
     * @see [io.seventeenninetyone.carlie.TcpServer.Connection.enableKeepAlive]
     * @see [io.seventeenninetyone.carlie.TcpServer.Connection.disableKeepAlive]
     */
    val isKeepAliveEnabled: Boolean
    /**
   * Get the local address of the connection.
   *
   * __Note:__ Returns `null` when the connection is closed.
   *
   * @see [io.seventeenninetyone.carlie.TcpServer.Address].
   * @see [io.seventeenninetyone.carlie.TcpServer.Connection.remoteAddress]
   */
    val localAddress: TcpServer.Address?
    /**
   * Get the remote address of the connection.
   *
   * __Note:__ Returns `null` when the connection is closed.
   *
   * @see [io.seventeenninetyone.carlie.TcpServer.Address].
   * @see [io.seventeenninetyone.carlie.TcpServer.Connection.localAddress]
   */
    val remoteAddress: TcpServer.Address?
    /**
     * Get the server.
     *
     * @see [io.seventeenninetyone.carlie.TcpServer].
     */
    val server: TcpServer

    /**
     * Close the connection and free up any resources that may be held by it.
     *
     * @see [java.lang.AutoCloseable.close]
     */
    override fun close()

    /**
     * Get a view of the connection as a [java.io.InputStream], for reading.
     *
     * @see [java.io.InputStream]
     */
    fun asInputStream(): InputStream

    /**
     * Get a view of the connection as a [java.io.OutputStream], for writing.
     *
     * @see [java.io.OutputStream]
     */
    fun asOutputStream(): OutputStream

    /**
     * Disable the TCP keep-alive functionality for the connection.
     *
     * @see [io.seventeenninetyone.carlie.TcpServer.Connection.enableKeepAlive]
     * @see [io.seventeenninetyone.carlie.TcpServer.Connection.isKeepAliveEnabled]
     */
    fun disableKeepAlive()

    /**
     * Enable the TCP keep-alive functionality for the connection.
     *
     * @param initialDelay The initial delay before the first keep-alive probe
     * is sent on an idle connection.
     * @see [io.seventeenninetyone.carlie.TcpServer.Connection.disableKeepAlive]
     * @see [io.seventeenninetyone.carlie.TcpServer.Connection.isKeepAliveEnabled]
     */
    fun enableKeepAlive(initialDelay: Int)

    /**
     * Enable the TCP keep-alive functionality for the connection.
     *
     * @param initialDelay The initial delay before the first keep-alive probe
     * is sent on an idle connection.
     * @see [io.seventeenninetyone.carlie.TcpServer.Connection.disableKeepAlive]
     * @see [io.seventeenninetyone.carlie.TcpServer.Connection.isKeepAliveEnabled]
     */
    @JvmSynthetic
    fun enableKeepAlive(initialDelay: UInt)

    /**
   * Attach an event handler for when the connection has closed.
   *
   * @param handler The event handler.
   * @see [io.seventeenninetyone.carlie.tcp_server.ClosedEventHandlerFunction]
   */
    fun onceClosed(handler: ClosedEventHandlerFunction)

    /**
     * @suppress
     */
    @JvmSynthetic
    fun onceClosed(handler: () -> Unit)

    /**
   * Attach an event handler for errors occurring on the connection.
   *
   * @param handler The event handler.
   * @see [io.seventeenninetyone.carlie.tcp_server.ErrorOccurredEventHandlerFunction]
   */
    fun onErrorOccurred(handler: ErrorOccurredEventHandlerFunction)

    /**
     * @suppress
     */
    @JvmSynthetic
    fun onErrorOccurred(handler: (@ParameterName("error") Throwable) -> Unit)

    /**
     * Read (asynchronously) from the connection’s underlying stream.
     *
     * @see [java.nio.channels.AsynchronousByteChannel.read]
     */
    @Throws(ClosedChannelException::class,
            ReadPendingException::class)
    override fun read(destinationBuffer: ByteBuffer): Future<Int>

    /**
     * Read (asynchronously) from the connection’s underlying stream.
     *
     * @see [java.nio.channels.AsynchronousByteChannel.read]
     */
    @Throws(ClosedChannelException::class,
            ReadPendingException::class)
    override fun <A> read(destinationBuffer: ByteBuffer,
                          attachment: A,
                          handler: CompletionHandler<Int, in A>)

    /**
   * Produce a string representation of the connection and its state.
   */
    override fun toString(): String

    /**
     * Write (asynchronously) to the connection’s underlying stream.
     *
     * @see [java.nio.channels.AsynchronousByteChannel.write]
     */
    @Throws(ClosedChannelException::class,
            WritePendingException::class)
    override fun write(sourceBuffer: ByteBuffer): Future<Int>

    /**
     * Write (asynchronously) to the connection’s underlying stream.
     *
     * @see [java.nio.channels.AsynchronousByteChannel.write]
     */
    @Throws(ClosedChannelException::class,
            WritePendingException::class)
    override fun <A> write(sourceBuffer: ByteBuffer,
                           attachment: A,
                           handler: CompletionHandler<Int, in A>)
  }

  private inner class ConnectionInternal : TcpServer.Connection {
    val isCloseable: Boolean
      @JvmSynthetic
      get() {
        return this.isCloseable(this.nativeObject)
      }

    @Volatile
    private var isClosed: Boolean

    private val isClosedOrClosing: Boolean
      get() {
        return (this.isClosed ||
                this.isClosing)
      }

    @Volatile
    private var isClosing: Boolean

    @Volatile
    override var isKeepAliveEnabled: Boolean
      private set

    private val nativeObject: ByteBuffer

    override val server: TcpServer
      get() {
        return this@TcpServer
      }

    private val events by lazy {
      EventEmitter()
    }

    private val handleClosedEventFunction by lazy {
      object : ClosedEventHandlerFunction {
        override fun handle() {
          this@TcpServer.threadPool.execute(Runnable {
            this@ConnectionInternal.finishClosing()
          })
        }
      }
    }

    private val handleClosedEventFunctionClass by lazy {
      this.handleClosedEventFunction::class.java
    }

    private val handleErrorOccurredEventFunction by lazy {
      object : ErrorOccurredEventHandlerFunction {
        override fun handle(error: Throwable) {
          this@TcpServer.threadPool.execute(Runnable {
            this@ConnectionInternal.emitErrorOccurredEvent(error)
          })
        }
      }
    }

    private val handleErrorOccurredEventFunctionClass by lazy {
      this.handleErrorOccurredEventFunction::class.java
    }

    override val id by lazy {
      val uuid = UUID.randomUUID()
      uuid!!
    }

    private val inputStream by lazy {
      Channels.newInputStream(this)
    }

    override val localAddress: TcpServer.Address? by object : ReadOnlyProperty<TcpServer.ConnectionInternal, TcpServer.Address?> {
      var address: TcpServer.Address?

      init {
        this.address = null
      }

      @Synchronized
      override fun getValue(thisRef: TcpServer.ConnectionInternal,
                            property: KProperty<*>): TcpServer.Address? {
        if (thisRef.isClosedOrClosing) {
          return null
        }
        if (this.address != null) {
          return this.address
        }
        val createAddressMethodFunction = this@TcpServer::createAddress
        val createAddressMethodFunctionClass = createAddressMethodFunction::class.java
        try {
          this.address = thisRef.getUvTcpBoundAddress(thisRef.nativeObject, createAddressMethodFunction, createAddressMethodFunctionClass)
        } catch (exception: UvException) {
          return null
        }
        return this.address
      }
    }

    private val outputStream by lazy {
      Channels.newOutputStream(this)
    }

    private val readLock by lazy {
      SimpleAtomicLock()
    }

    override val remoteAddress: TcpServer.Address? by object : ReadOnlyProperty<TcpServer.ConnectionInternal, TcpServer.Address?> {
      var address: TcpServer.Address?

      init {
        this.address = null
      }

      @Synchronized
      override fun getValue(thisRef: TcpServer.ConnectionInternal,
                            property: KProperty<*>): TcpServer.Address? {
        if (thisRef.isClosedOrClosing) {
          return null
        }
        if (this.address != null) {
          return this.address
        }
        val createAddressMethodFunction = this@TcpServer::createAddress
        val createAddressMethodFunctionClass = createAddressMethodFunction::class.java
        try {
          this.address = thisRef.getUvTcpRemoteAddress(thisRef.nativeObject, createAddressMethodFunction, createAddressMethodFunctionClass)
        } catch (exception: UvException) {
          return null
        }
        return this.address
      }
    }

    private val writeLock by lazy {
      SimpleAtomicLock()
    }

    constructor(nativeObject: ByteBuffer) {
      this.isClosed = false
      this.isClosing = false
      this.isKeepAliveEnabled = false
      this.nativeObject = nativeObject
      this@TcpServer.closeFlagReadWriteLock.read {
        // TODO: Once logging is set-up, log about this.
        if (this@TcpServer.isClosedOrClosing) return
        this@TcpServer.addConnection(this)
        this.handleNotCloseableErrors()
        val closeMethodFunction = this::close
        val closeMethodFunctionClass = closeMethodFunction::class.java
        val nativeIsInitialized = this.initializeNative(this.nativeObject, this@TcpServer.nativeObject, closeMethodFunction, closeMethodFunctionClass, this.handleClosedEventFunction, this.handleClosedEventFunctionClass, this.handleErrorOccurredEventFunction, this.handleErrorOccurredEventFunctionClass)
        // TODO: Once logging is set-up, log about this.
        if (! nativeIsInitialized) return
        try {
          this.initializeUvTcpHandle(this.nativeObject)
        } catch (exception: UvException) {
          this.use {
            this@TcpServer.emitErrorOccurredEvent(exception)
          }
          return
        }
        this.enableKeepAlive(0u)
      }
    }

    private external fun initializeNative(connectionNativeObject: ByteBuffer,
                                          serverNativeObject: ByteBuffer,
                                          closeMethodFunction: Function0<Unit>,
                                          closeMethodFunctionClass: Class<out Function0<Unit>>,
                                          handleClosedEventFunction: ClosedEventHandlerFunction,
                                          handleClosedEventFunctionClass: Class<out ClosedEventHandlerFunction>,
                                          handleErrorOccurredEventFunction: ErrorOccurredEventHandlerFunction,
                                          handleErrorOccurredEventFunctionClass: Class<out ErrorOccurredEventHandlerFunction>): Boolean

    @Synchronized
    override fun close() {
      if (this.isClosedOrClosing) return
      if (! this.isCloseable) {
        this.emitNotCloseableErrorEvent()
        return
      }
      this@TcpServer.closeFlagReadWriteLock.read {
        if (this@TcpServer.isClosedOrClosing) return
        try {
          this.closeUvTcpHandle(this.nativeObject)
        } catch (exception: UvException) {
          this.emitNotCloseableErrorEvent()
          return
        }
        this.isClosing = true
      }
    }

    private external fun closeNative(nativeObject: ByteBuffer)

    protected fun finalize() {
      this.close()
    }

    override fun asInputStream(): InputStream {
      return this.inputStream
    }

    override fun asOutputStream(): OutputStream {
      return this.outputStream
    }

    @Synchronized
    private external fun closeInUvWorker(nativeObject: ByteBuffer)

    private external fun closeUvTcpHandle(nativeObject: ByteBuffer)

    override fun disableKeepAlive() {
      if (this.isClosedOrClosing) return
      if (! this.isKeepAliveEnabled) return
      try {
        this.uvTcpDisableKeepAlive(this.nativeObject)
      } catch(exception: UvException) {
        return
      }
      this.isKeepAliveEnabled = false
    }

    private fun emitClosedEvent() {
      this.events.emitEvent(TcpServer.CLOSED_EVENT_NAME)
    }

    private fun emitErrorOccurredEvent(error: Throwable) {
      this.events.emitEvent(TcpServer.ERROR_OCCURRED_EVENT_NAME, error)
    }

    private fun emitNotCloseableErrorEvent() {
      this.events.emitEvent(TcpServer.CLIENT_NOT_CLOSEABLE_ERROR_EVENT_NAME)
    }

    override fun enableKeepAlive(initialDelay: Int) {
      return this.enableKeepAlive(when {
        (initialDelay >= 0) -> initialDelay.toUInt()
        else -> 0u
      })
    }

    @JvmSynthetic
    override fun enableKeepAlive(initialDelay: UInt) {
      if (this.isClosedOrClosing) return
      if (this.isKeepAliveEnabled) return
      try {
        this.uvTcpEnableKeepAlive(this.nativeObject, when {
          (initialDelay <= Int.MAX_VALUE.toUInt()) -> initialDelay.toInt()
          else -> Int.MAX_VALUE
        })
      } catch(exception: UvException) {
        return
      }
      this.isKeepAliveEnabled = true
    }

    @Synchronized
    private fun finishClosing() {
      if (this.isClosed) return
      if (! this.isClosing) return
      this.closeNative(this.nativeObject)
      this.nativeObject.clear()
      this.isClosing = false
      this.isClosed = true
      this@TcpServer.removeConnection(this)
      this.emitClosedEvent()
      this.events.removeAllEventHandlers()
    }

    @Throws(UvException::class)
    private external fun getUvTcpBoundAddress(nativeObject: ByteBuffer,
                                              createAddressMethodFunction: Function3<String, Int, Int, TcpServer.AddressInternal>,
                                              createAddressMethodFunctionClass: Class<out Function3<String, Int, Int, TcpServer.AddressInternal>>): TcpServer.AddressInternal

    @Throws(UvException::class)
    private external fun getUvTcpRemoteAddress(nativeObject: ByteBuffer,
                                               createAddressMethodFunction: Function3<String, Int, Int, TcpServer.AddressInternal>,
                                               createAddressMethodFunctionClass: Class<out Function3<String, Int, Int, TcpServer.AddressInternal>>): TcpServer.AddressInternal

    private fun handleNotCloseableErrors() {
      this.events.onEvent(TcpServer.CLIENT_NOT_CLOSEABLE_ERROR_EVENT_NAME, object : EventHandlerFunction {
        override fun handle(data: Any?) {
          this@ConnectionInternal.closeInUvWorker(this@ConnectionInternal.nativeObject)
        }
      })
    }

    @Throws(UvException::class)
    private external fun initializeUvTcpHandle(nativeObject: ByteBuffer)

    @Synchronized
    private external fun isCloseable(nativeObject: ByteBuffer): Boolean

    override fun isOpen(): Boolean {
      return (! this.isClosedOrClosing)
    }

    override fun onceClosed(handler: ClosedEventHandlerFunction) {
      return this.onceClosedEvent(handler)
    }

    @JvmSynthetic
    override fun onceClosed(handler: () -> Unit) {
      return this.onceClosed(object : ClosedEventHandlerFunction {
        override fun handle() {
          return handler()
        }
      })
    }

    private fun onceClosedEvent(handler: ClosedEventHandlerFunction) {
      if (this.isClosed) return
      this.events.onceEvent(TcpServer.CLOSED_EVENT_NAME, object : EventHandlerFunction {
        override fun handle(data: Any?) {
          handler()
        }
      })
    }

    override fun onErrorOccurred(handler: ErrorOccurredEventHandlerFunction) {
      return this.onErrorOccurredEvent(handler)
    }

    @JvmSynthetic
    override fun onErrorOccurred(handler: (@ParameterName("error") Throwable) -> Unit) {
      return this.onErrorOccurred(object : ErrorOccurredEventHandlerFunction {
        override fun handle(error: Throwable) {
          return handler(error)
        }
      })
    }

    private fun onErrorOccurredEvent(handler: ErrorOccurredEventHandlerFunction) {
      if (this.isClosedOrClosing) return
      this.events.onEvent(TcpServer.ERROR_OCCURRED_EVENT_NAME, object : EventHandlerFunction {
        override fun handle(data: Any?) {
          handler(data as Throwable)
          when (data) {
            is UvException -> {
              this@ConnectionInternal.close()
            }
          }
        }
      })
    }

    @Throws(ClosedChannelException::class,
            ReadPendingException::class)
    override fun read(destinationBuffer: ByteBuffer): Future<Int> {
      val futureResult: CompletableFuture<Int> = CompletableFuture()
      this@ConnectionInternal.read(destinationBuffer, Unit, object : CompletionHandler<Int, Unit> {
        override fun completed(result: Int,
                               attachment: Unit) {
          futureResult.complete(result)
        }

        override fun failed(exception: Throwable,
                            attachment: Unit) {}
      })
      return futureResult
    }

    @Throws(ClosedChannelException::class,
            ReadPendingException::class)
    override fun <A> read(destinationBuffer: ByteBuffer,
                          attachment: A,
                          handler: CompletionHandler<Int, in A>) {
      if (this.isClosedOrClosing) {
        throw ClosedChannelException()
      }
      if (destinationBuffer.isReadOnly()) {
        throw IllegalArgumentException()
      }
      val readLockIsAcquired = this.readLock.tryLock()
      if (! readLockIsAcquired) {
        throw ReadPendingException()
      }
      var keepReadLockLocked = false
      try {
        val remainingDestinationBufferBytesCount = destinationBuffer.remaining()
        if (remainingDestinationBufferBytesCount == 0) {
          handler.completed(0, attachment)
          return
        }
        val bufferSize = remainingDestinationBufferBytesCount
        val destinationBufferPosition = destinationBuffer.position()
        val buffer = ByteArray(bufferSize)
        val callback = l@{
          bytesReadCount: Int?,
          error: UvException? ->
            this.readLock.unlock()
            if (error != null) {
              this.emitErrorOccurredEvent(error)
              // NOTE: `handler.completed(0, …)` is called on failure rather
              // than `handler.failed(…)` since the error is being handled via
              // the event system instead.
              handler.completed(0, attachment)
              return@l
            }
            bytesReadCount!!
            when {
              (bytesReadCount > 0) -> {
                for (i in 0 until bytesReadCount) {
                  val destinationBufferIndex = destinationBufferPosition + i
                  destinationBuffer.put(destinationBufferIndex, buffer[i])
                }
                destinationBuffer.position(destinationBufferPosition + bytesReadCount)
                handler.completed(bytesReadCount, attachment)
              }
              (bytesReadCount == 0) -> {
                // No bytes were read, unfortunately.
                handler.completed(0, attachment)
              }
              else -> {
                // EOF (end-of-stream) has been reached.
                handler.completed(-1, attachment)
                this.close()
              }
            }
        }
        val callbackClass = callback::class.java
        try {
          this.uvTcpRead(this.nativeObject, buffer, bufferSize, callback, callbackClass)
        } catch (exception: UvException) {
          this.emitErrorOccurredEvent(exception)
          return
        }
        keepReadLockLocked = true
      } finally {
        if (! keepReadLockLocked) {
          this.readLock.unlock()
        }
      }
    }

    override fun toString(): String {
      val prefix = "TCP client connection {"
      val suffix = "}"
      return when {
        this.isClosing -> "${prefix}status=CLOSING${suffix}"
        this.isClosed -> "${prefix}status=CLOSED${suffix}"
        else -> {
          val (localIpAddress, localAddressVersion, localAddressPort) = this.localAddress!!
          val localAddress = "(IP=${localIpAddress}, version=${localAddressVersion}, port=${localAddressPort})"
          val (remoteIpAddress, remoteAddressVersion, remoteAddressPort) = this.remoteAddress!!
          val remoteAddress = "(IP=${remoteIpAddress}, version=${remoteAddressVersion}, port=${remoteAddressPort})"
          val spacePrefix = {
            val spaces = CharArray(prefix.length) {
              ' '
            }
            spaces.joinToString("")
          }()
          var result = ""
          result += prefix
          result += "status=CONNECTED"
          result += ",\n"
          result += spacePrefix
          result += "localAddress=${localAddress}"
          result += ",\n"
          result += spacePrefix
          result += "remoteAddress=${remoteAddress}"
          result += suffix
          result
        }
      }
    }

    @Throws(UvException::class)
    @Synchronized
    private external fun uvTcpDisableKeepAlive(nativeObject: ByteBuffer)

    @Throws(UvException::class)
    @Synchronized
    private external fun uvTcpEnableKeepAlive(nativeObject: ByteBuffer,
                                              initialDelay: Int)

    @Throws(UvException::class)
    private external fun uvTcpRead(nativeObject: ByteBuffer,
                                   buffer: ByteArray,
                                   bufferSize: Int,
                                   callback: Function2<Int, UvException?, Unit>,
                                   callbackClass: Class<out Function2<Int, UvException?, Unit>>)

    @Throws(UvException::class)
    private external fun uvTcpWrite(nativeObject: ByteBuffer,
                                    buffer: ByteArray,
                                    bufferSize: Int,
                                    callback: Function2<Int, UvException?, Unit>,
                                    callbackClass: Class<out Function2<Int, UvException?, Unit>>)

    @Throws(ClosedChannelException::class,
            WritePendingException::class)
    override fun write(sourceBuffer: ByteBuffer): Future<Int> {
      val futureResult: CompletableFuture<Int> = CompletableFuture()
      this@ConnectionInternal.write(sourceBuffer, Unit, object : CompletionHandler<Int, Unit> {
        override fun completed(result: Int,
                               attachment: Unit) {
          futureResult.complete(result)
        }

        override fun failed(exception: Throwable,
                            attachment: Unit) {}
      })
      return futureResult
    }

    @Throws(ClosedChannelException::class,
            WritePendingException::class)
    override fun <A> write(sourceBuffer: ByteBuffer,
                           attachment: A,
                           handler: CompletionHandler<Int, in A>) {
      if (this.isClosedOrClosing) {
        throw ClosedChannelException()
      }
      val writeLockIsAcquired = this.writeLock.tryLock()
      if (! writeLockIsAcquired) {
        throw WritePendingException()
      }
      var keepWriteLockLocked = false
      try {
        val remainingSourceBufferBytesCount = sourceBuffer.remaining()
        if (remainingSourceBufferBytesCount == 0) {
          handler.completed(0, attachment)
          return
        }
        val bufferSize = remainingSourceBufferBytesCount
        val sourceBufferPosition = sourceBuffer.position()
        // TODO: It might be more efficient to always just create a new byte
        // array directly, regardless if the source buffer has an accessible
        // array (the slice operation still creates a new array anyway). Look
        // into this.
        val buffer = when {
          sourceBuffer.hasArray() -> {
            val array = sourceBuffer.array()
            array!!
            val arrayOffset = sourceBuffer.arrayOffset()
            val arrayPosition = arrayOffset + sourceBufferPosition
            val lastArrayIndex = arrayPosition + (bufferSize - 1)
            array.sliceArray(arrayPosition..lastArrayIndex)
          }
          else -> {
            ByteArray(bufferSize) {
              index ->
                sourceBuffer.get(sourceBufferPosition + index)
            }
          }
        }
        val callback = l@{
          bytesWrittenCount: Int?,
          error: UvException? ->
            this.writeLock.unlock()
            if (error != null) {
              this.emitErrorOccurredEvent(error)
              // NOTE: `handler.completed(0, …)` is called on failure rather
              // than `handler.failed(…)` since the error is being handled via
              // the event system instead.
              handler.completed(0, attachment)
              return@l
            }
            bytesWrittenCount!!
            sourceBuffer.position(sourceBufferPosition + bytesWrittenCount)
            handler.completed(bytesWrittenCount, attachment)
        }
        val callbackClass = callback::class.java
        try {
          this.uvTcpWrite(this.nativeObject, buffer, bufferSize, callback, callbackClass)
        } catch (exception: UvException) {
          this.emitErrorOccurredEvent(exception)
          return
        }
        keepWriteLockLocked = true
      } finally {
        if (! keepWriteLockLocked) {
          this.writeLock.unlock()
        }
      }
    }
  }
}
