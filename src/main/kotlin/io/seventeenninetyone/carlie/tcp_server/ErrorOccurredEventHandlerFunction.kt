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

/**
 * The functional interface for a function used as an event handler for errors
 * occurring on a server or errors occurring on a server’s connections.
 *
 * @author Jay B.
 * @see [io.seventeenninetyone.carlie.TcpServer.onErrorOccurred]
 * @see [io.seventeenninetyone.carlie.TcpServer.Connection.onErrorOccurred]
 */
@FunctionalInterface
interface ErrorOccurredEventHandlerFunction {
  fun handle(error: Throwable)

  @JvmSynthetic
  @JvmDefault
  operator fun invoke(error: Throwable) {
    return this.handle(error)
  }
}
