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

package io.seventeenninetyone.carlie.utilities.native_library_loader

class Platform {
  companion object {
    val ARCHITECTURE: String?
    val OPERATING_SYSTEM: String?

    init {
      when {
        (com.sun.jna.Platform.isLinux() &&
         com.sun.jna.Platform.is64Bit()) -> {
          this.ARCHITECTURE = when {
            com.sun.jna.Platform.isARM() -> "aarch64"
            com.sun.jna.Platform.isIntel() -> "x86_64"
            else -> null
          }
          this.OPERATING_SYSTEM = "linux"
        }
        (com.sun.jna.Platform.isMac() &&
         com.sun.jna.Platform.isIntel() &&
         com.sun.jna.Platform.is64Bit()) -> {
          this.ARCHITECTURE = "x86_64"
          this.OPERATING_SYSTEM = "mac"
        }
        (com.sun.jna.Platform.isWindows() &&
         com.sun.jna.Platform.isIntel()) -> {
          this.ARCHITECTURE = (
            if (com.sun.jna.Platform.is64Bit()) {
              "x86_64"
            } else {
              "x86"
            }
          )
          this.OPERATING_SYSTEM = "windows"
        }
        else -> {
          this.ARCHITECTURE = null
          this.OPERATING_SYSTEM = null
        }
      }
    }

    val sluggifiedName by lazy {
      com.sun.jna.Platform.RESOURCE_PREFIX!!
    }

    @JvmStatic
    fun isSupported(): Boolean {
      return ((this.ARCHITECTURE != null) &&
              (this.OPERATING_SYSTEM != null))
    }
  }
}
