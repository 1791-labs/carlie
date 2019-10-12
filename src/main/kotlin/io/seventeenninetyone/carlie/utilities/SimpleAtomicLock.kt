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

import java.util.concurrent.atomic.AtomicBoolean

class SimpleAtomicLock {
  val isLocked by lazy {
    this.value.get()
  }

  private val value by lazy {
    AtomicBoolean(false)
  }

  fun tryLock(): Boolean {
    return this.value.compareAndSet(false, true)
  }

  @Throws(IllegalStateException::class)
  fun unlock() {
    val valueUpdated = this.value.compareAndSet(true, false)
    // If the lock was already acquired, then throw an exception.
    if (! valueUpdated) {
      throw IllegalStateException()
    }
  }
}
