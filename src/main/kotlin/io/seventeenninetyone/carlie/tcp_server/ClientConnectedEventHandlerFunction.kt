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

package io.seventeenninetyone.carlie.tcp_server

import io.seventeenninetyone.carlie.TcpServer

/**
 * The functional interface for a function used as an event handler for incoming
 * connections on a server.
 *
 * @author Jay B.
 * @see [io.seventeenninetyone.carlie.TcpServer.onClientConnected]
 */
@FunctionalInterface
interface ClientConnectedEventHandlerFunction {
  fun handle(connection: TcpServer.Connection)

  @JvmSynthetic
  @JvmDefault
  operator fun invoke(connection: TcpServer.Connection) {
    return this.handle(connection)
  }
}
