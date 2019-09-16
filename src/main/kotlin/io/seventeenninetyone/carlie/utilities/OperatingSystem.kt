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

import java.lang.reflect.Field
import sun.misc.Unsafe

class OperatingSystem {
  companion object {
    @JvmStatic val pageSize: UInt by lazy getPageSize@ {
      if (this.UNSAFE_CLASS == null) {
        return@getPageSize 0.toUInt()
      }
      // http://www.docjar.com/docs/api/sun/misc/Unsafe.html#pageSize
      var size = this.UNSAFE_CLASS!!.pageSize()
      // Is this actually possible? Does the `pageSize` method provide any sign
      // guarantees?
      // NOTE: Just being safe since we are dealing with signed integers here
      // and the `pageSize` method is a native method.
      if (size < 0) {
        size = 0
      }
      size.toUInt()
    }
    private val UNSAFE_CLASS: Unsafe? by lazy getUnsafeClass@ {
      val unsafeClassTheUnsafeField: Field
      try {
        unsafeClassTheUnsafeField = this.UNSAFE_CLASS_CLASS.getDeclaredField("theUnsafe")
      } catch (exception: NoSuchFieldException) {
        return@getUnsafeClass null
      } catch (exception: SecurityException) {
        return@getUnsafeClass null
      }
      try {
        unsafeClassTheUnsafeField.isAccessible = true
      } catch (exception: SecurityException) {
        return@getUnsafeClass null
      }
      val unsafeClass: Unsafe
      try {
        unsafeClass = unsafeClassTheUnsafeField.get(null) as Unsafe
      } catch (exception: IllegalAccessException) {
        return@getUnsafeClass null
      }
      unsafeClass
    }
    private val UNSAFE_CLASS_CLASS = Unsafe::class.java

    @JvmStatic fun getPageSizeOrDefault(defaultSize: Int): Int {
      return this.getPageSizeOrDefault(defaultSize.toUInt()).toInt()
    }

    @JvmStatic fun getPageSizeOrDefault(defaultSize: UInt): UInt {
      return (
        if (this.pageSize > 0.toUInt()) {
          this.pageSize
        } else {
          defaultSize
        }
      )
    }
  }
}
