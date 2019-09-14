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

typealias EventHandler = EventHandlerFunction<@ParameterName(name = "data") Any?, Void?>
typealias EventHandlerBase = Function1<@ParameterName(name = "data") Any?, Void?>

class EventEmitter {
  private var eventCount: ULong
  private var handlers: MutableMap<String, MutableList<EventHandlerBase>>
  private var onceHandlers: MutableMap<EventHandlerBase, EventHandlerBase>

  constructor() {
    this.eventCount = 0uL
    this.handlers = Maps.newConcurrentMap()
    this.onceHandlers = Maps.newConcurrentMap()
  }

  @Throws(EventEmitterInvalidEventNameException::class)
  fun emitEvent(name: String) = this.emitEvent(name, null)

  @Throws(EventEmitterInvalidEventNameException::class)
  fun emitEvent(name: String,
                data: Any?) {
    val name1 = name.trim()
    if (name1 == "") {
      throw EventEmitterInvalidEventNameException()
    }
    val handlers = this.handlers[name1]
    if (handlers == null) return
    if (handlers.isEmpty()) return
    handlers.forEach {
      handler ->
        handler(data)
    }
  }

  fun getEventHandlersFor(name: String): Array<EventHandler> {
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
            onceHandler as EventHandler
          } else {
            handler as EventHandler
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

  @Throws(EventEmitterInvalidEventNameException::class)
  fun onceEvent(name: String,
                handler: EventHandler) {
    return this.onceEvent(name, handler as EventHandlerBase)
  }

  @Throws(EventEmitterInvalidEventNameException::class)
  fun onceEvent(name: String,
                handler: EventHandlerBase) {
    fun wrappedHandler(data: Any?): Void? {
      this.removeEventHandlerFor(name, ::wrappedHandler)
      this.onceHandlers.remove(::wrappedHandler)
      handler(data)
      return null
    }
    this.onceHandlers[::wrappedHandler] = handler
    this.onEvent(name, ::wrappedHandler)
  }

  @Throws(EventEmitterInvalidEventNameException::class)
  fun onEvent(name: String,
              handler: EventHandler) {
    return this.onEvent(name, handler as EventHandlerBase)
  }

  @Throws(EventEmitterInvalidEventNameException::class)
  fun onEvent(name: String,
              handler: EventHandlerBase) {
    val name1 = name.trim()
    if (name1 == "") {
      throw EventEmitterInvalidEventNameException()
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
                            handler: EventHandler) {
    return this.removeEventHandlerFor(name, handler as EventHandlerBase)
  }

  fun removeEventHandlerFor(name: String,
                            handler: EventHandlerBase) {
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
