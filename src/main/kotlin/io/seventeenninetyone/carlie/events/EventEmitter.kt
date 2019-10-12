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

package io.seventeenninetyone.carlie.events

import com.google.common.collect.Lists
import com.google.common.collect.Maps
import io.seventeenninetyone.carlie.events.event_emitter.EventHandlerFunction
import io.seventeenninetyone.carlie.events.event_emitter.InvalidEventNameException

class EventEmitter {
  private var eventCount: ULong
  private var handlers: MutableMap<String, MutableList<EventHandlerFunction>>
  private var onceHandlers: MutableMap<EventHandlerFunction, EventHandlerFunction>

  constructor() {
    this.eventCount = 0uL
    this.handlers = Maps.newConcurrentMap()
    this.onceHandlers = Maps.newConcurrentMap()
  }

  @Throws(InvalidEventNameException::class)
  fun emitEvent(name: String) {
    this.emitEvent(name, null)
  }

  @Throws(InvalidEventNameException::class)
  fun emitEvent(name: String,
                data: Any?) {
    val name1 = name.trim()
    if (name1 == "") {
      throw InvalidEventNameException()
    }
    val handlers = this.handlers[name1]
    if (handlers == null) return
    if (handlers.isEmpty()) return
    handlers.forEach {
      handler ->
        handler(data)
    }
  }

  fun getEventHandlersFor(name: String): Array<EventHandlerFunction> {
    val handlers = this.handlers[name]
    if (handlers == null) {
      return emptyArray()
    }
    if (handlers.isEmpty()) {
      return emptyArray()
    }
    return handlers
      .map {
        handler ->
          val onceHandler = this.onceHandlers[handler]
          if (onceHandler != null) {
            onceHandler
          } else {
            handler
          }
      }
      .toTypedArray()
  }

  fun getEventNames(): Array<String> {
    if (this.eventCount == 0uL) {
      return emptyArray()
    }
    val handlersKeys = this.handlers.keys
    return handlersKeys.toTypedArray()
  }

  @Throws(InvalidEventNameException::class)
  fun onceEvent(name: String,
                handler: EventHandlerFunction) {
    // fun wrappedHandler(data: Any?) {
    //   val wrappedHandler = ::wrappedHandler as EventHandlerFunction
    //   this.removeEventHandlerFor(name, wrappedHandler)
    //   this.onceHandlers.remove(wrappedHandler)
    //   handler(data)
    // }
    // val wrappedHandler = ::wrappedHandler as EventHandlerFunction
    // this.onceHandlers[wrappedHandler] = handler
    // this.onEvent(name, wrappedHandler)
    val wrappedHandlerBox: Array<EventHandlerFunction?> = arrayOf(null)
    val wrappedHandler = (object : EventHandlerFunction {
      override fun handle(data: Any?) {
        val wrappedHandler = wrappedHandlerBox[0]!!
        wrappedHandlerBox[0] = null
        this@EventEmitter.removeEventHandlerFor(name, wrappedHandler)
        this@EventEmitter.onceHandlers.remove(wrappedHandler)
        handler(data)
      }
    }) as EventHandlerFunction
    wrappedHandlerBox[0] = wrappedHandler
    this.onceHandlers[wrappedHandler] = handler
    this.onEvent(name, wrappedHandler)
  }

  @Throws(InvalidEventNameException::class)
  fun onEvent(name: String,
              handler: EventHandlerFunction) {
    val name1 = name.trim()
    if (name1 == "") {
      throw InvalidEventNameException()
    }
    var handlers = this.handlers[name1]
    if (handlers == null) {
      handlers = Lists.newLinkedList()
      this.handlers[name1] = handlers
      // New event, so update the event count.
      this.eventCount++
    }
    handlers!!.add(handler)
  }

  fun removeAllEventHandlers() {
    this.eventCount = 0uL
    this.handlers.clear()
  }

  fun removeEventHandlerFor(name: String,
                            handler: EventHandlerFunction) {
    val handlers = this.handlers[name]
    if (handlers == null) return
    if (handlers.isEmpty()) return
    val handlerLastIndex = handlers.lastIndexOf(handler)
    if (handlerLastIndex == -1) return
    handlers.removeAt(handlerLastIndex)
  }

  fun removeEventHandlersFor(name: String) {
    val handlers = this.handlers[name]
    if (handlers == null) return
    handlers.clear()
  }
}
