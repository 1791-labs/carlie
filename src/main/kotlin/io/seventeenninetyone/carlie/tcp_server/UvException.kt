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

import com.google.common.collect.Maps
import io.seventeenninetyone.carlie.utilities.NativeLibraryLoader

/**
 * An exception thrown when a libuv-related error occurs.
 *
 * @author Jay B.
 */
class UvException : Exception {
  companion object {
    private val errors: MutableMap<Int, Pair<String, String>>
    const val serialVersionUID = 1L

    private val integerClass by lazy {
      java.lang.Integer::class.java
    }

    private val mapClass by lazy {
      java.util.Map::class.java
    }

    private val pairClass by lazy {
      Pair::class.java
    }

    init {
      try {
        NativeLibraryLoader.extractAndLoad()
      } catch (error: Throwable) {
        NativeLibraryLoader.abort()
      }
      this.errors = Maps.newHashMap()
      this.loadErrors(this.errors, this.mapClass, this.integerClass, this.pairClass)
    }

    @Synchronized
    @JvmStatic
    private external fun loadErrors(map: MutableMap<Int, Pair<String, String>>,
                                    mapClass: Class<java.util.Map<*, *>>,
                                    integerClass: Class<java.lang.Integer>,
                                    pairClass: Class<Pair<*, *>>): Boolean

    private fun processClassConstructorArguments(errorNumber: Int): String {
      val errorPair = this.errors[errorNumber]
      if (errorPair == null) {
        // This should never be reached.
        return "strange unknown error"
      }
      val (errorName, errorMessage) = errorPair
      return "[$errorName] → $errorMessage"
    }
  }

  /**
   * @param errorNumber The libuv error code.
   */
  constructor(errorNumber: Int):
    super(UvException.processClassConstructorArguments(errorNumber))
}
