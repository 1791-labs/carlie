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

package io.seventeenninetyone.carlie.utilities

import com.sun.jna.Platform

class NativeLibraryLoaderPlatform {
  companion object {
    val ARCHITECTURE: String?
    val OS: String?

    init {
      when {
        (Platform.isLinux() &&
         Platform.is64Bit()) -> {
          this.OS = "linux"
          this.ARCHITECTURE = when {
            Platform.isARM() -> "aarch64"
            Platform.isIntel() -> "x86_64"
            else -> null
          }
        }
        (Platform.isMac() &&
         Platform.isIntel() &&
         Platform.is64Bit()) -> {
          this.ARCHITECTURE = "x86_64"
          this.OS = "mac"
        }
        (Platform.isWindows() &&
         Platform.isIntel()) -> {
          this.OS = "windows"
          this.ARCHITECTURE = (
            if (Platform.is64Bit()) {
              "x86_64"
            } else {
              "x86"
            }
          )
        }
        else -> {
          this.ARCHITECTURE = null
          this.OS = null
        }
      }
    }

    @JvmStatic fun isSupported(): Boolean {
      return ((this.ARCHITECTURE != null) &&
              (this.OS != null))
    }
  }
}
