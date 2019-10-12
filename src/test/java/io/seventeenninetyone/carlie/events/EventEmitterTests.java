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

package io.seventeenninetyone.carlie.events;

import io.seventeenninetyone.carlie.events.event_emitter.EventHandlerFunction;
import io.seventeenninetyone.carlie.events.event_emitter.InvalidEventNameException;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Test;

import java.text.Collator;
import java.util.Arrays;
import java.util.Locale;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

@DisplayName("EventEmitter Unit Tests")
class EventEmitterTests
{
  private final static Collator enUsCollator;
  static {
    final Locale locale = new Locale("en", "US");
    enUsCollator = Collator.getInstance(locale);
  }

  @Test
  @DisplayName("new EventEmitter()")
  void testConstructor()
  {
    final EventEmitter events = new EventEmitter();
    final String[] eventNames = events.getEventNames();
    assertEquals(0, eventNames.length);
  }

  @Test
  @DisplayName("EventEmitter#getEventNames()")
  void testGetEventNames()
    throws InvalidEventNameException
  {
    final EventEmitter events = new EventEmitter();
    String[] eventNames;
    eventNames = events.getEventNames();
    assertEquals(0, eventNames.length);
    final String eventName1 = "bar";
    events.onEvent(eventName1, (data) -> {});
    eventNames = events.getEventNames();
    assertEquals(1, eventNames.length);
    assertEquals(eventName1, eventNames[0]);
    final String eventName2 = "baz";
    events.onEvent(eventName2, (data) -> {});
    eventNames = events.getEventNames();
    Arrays.sort(eventNames, EventEmitterTests.enUsCollator);
    assertEquals(2, eventNames.length);
    assertEquals(eventName1, eventNames[0]);
    assertEquals(eventName2, eventNames[1]);
    final String eventName3 = "foo";
    events.onEvent(eventName3, (data) -> {});
    eventNames = events.getEventNames();
    Arrays.sort(eventNames, EventEmitterTests.enUsCollator);
    assertEquals(3, eventNames.length);
    assertEquals(eventName1, eventNames[0]);
    assertEquals(eventName2, eventNames[1]);
    assertEquals(eventName3, eventNames[2]);
  }

  @Test
  @DisplayName("EventEmitter#getEventHandlersFor(…)")
  void testGetEventHandlersFor()
    throws InvalidEventNameException
  {
    final EventEmitter events = new EventEmitter();
    final String eventName = "bar";
    EventHandlerFunction[] eventHandlers;
    eventHandlers = events.getEventHandlersFor(eventName);
    assertEquals(0, eventHandlers.length);
    final EventHandlerFunction eventHandler1 = (data) -> {};
    events.onEvent(eventName, eventHandler1);
    eventHandlers = events.getEventHandlersFor(eventName);
    assertEquals(1, eventHandlers.length);
    assertEquals(eventHandler1, eventHandlers[0]);
    final EventHandlerFunction eventHandler2 = (data) -> {};
    events.onEvent(eventName, eventHandler2);
    eventHandlers = events.getEventHandlersFor(eventName);
    assertEquals(2, eventHandlers.length);
    assertEquals(eventHandler1, eventHandlers[0]);
    assertEquals(eventHandler2, eventHandlers[1]);
    events.removeEventHandlersFor(eventName);
    eventHandlers = events.getEventHandlersFor(eventName);
    assertEquals(0, eventHandlers.length);
  }

  @Test
  @DisplayName("EventEmitter#onEvent(…)")
  void testOnEvent()
    throws InvalidEventNameException
  {
    final EventEmitter events = new EventEmitter();
    String[] eventNames;
    eventNames = events.getEventNames();
    assertEquals(0, eventNames.length);
    final String eventName1 = "bar";
    final EventHandlerFunction eventHandler1 = (data) -> {};
    events.onEvent(eventName1, eventHandler1);
    eventNames = events.getEventNames();
    assertEquals(1, eventNames.length);
    assertEquals(eventName1, eventNames[0]);
    EventHandlerFunction[] eventHandlers;
    eventHandlers = events.getEventHandlersFor(eventName1);
    assertEquals(1, eventHandlers.length);
    assertEquals(eventHandler1, eventHandlers[0]);
    final EventHandlerFunction eventHandler2 = (data) -> {};
    events.onEvent(eventName1, eventHandler2);
    eventHandlers = events.getEventHandlersFor(eventName1);
    assertEquals(2, eventHandlers.length);
    assertEquals(eventHandler1, eventHandlers[0]);
    assertEquals(eventHandler2, eventHandlers[1]);
    final String eventName2 = "baz";
    final EventHandlerFunction eventHandler3 = (data) -> {};
    events.onEvent(eventName2, eventHandler3);
    eventNames = events.getEventNames();
    Arrays.sort(eventNames, EventEmitterTests.enUsCollator);
    assertEquals(2, eventNames.length);
    assertEquals(eventName1, eventNames[0]);
    assertEquals(eventName2, eventNames[1]);
    eventHandlers = events.getEventHandlersFor(eventName2);
    assertEquals(1, eventHandlers.length);
    assertEquals(eventHandler3, eventHandlers[0]);
    final EventHandlerFunction eventHandler4 = (data) -> {};
    assertThrows(InvalidEventNameException.class, () -> {
      events.onEvent("   ", eventHandler4);
    });
    assertThrows(InvalidEventNameException.class, () -> {
      events.onEvent("", eventHandler4);
    });
    assertThrows(InvalidEventNameException.class, () -> {
      events.onEvent(" \t ", eventHandler4);
    });
    assertThrows(InvalidEventNameException.class, () -> {
      events.onEvent(" \n ", eventHandler4);
    });
  }

  @Test
  @DisplayName("EventEmitter#onceEvent(…)")
  void testOnceEvent()
    throws InvalidEventNameException
  {
    final EventEmitter events = new EventEmitter();
    final String eventName = "bar";
    EventHandlerFunction[] eventHandlers;
    eventHandlers = events.getEventHandlersFor(eventName);
    assertEquals(0, eventHandlers.length);
    final EventHandlerFunction eventHandler = (data) -> {};
    events.onceEvent(eventName, eventHandler);
    eventHandlers = events.getEventHandlersFor(eventName);
    assertEquals(1, eventHandlers.length);
    assertEquals(eventHandler, eventHandlers[0]);
    events.emitEvent(eventName);
    eventHandlers = events.getEventHandlersFor(eventName);
    assertEquals(0, eventHandlers.length);
  }

  @Test
  @DisplayName("EventEmitter#emitEvent(…)")
  void testEmitEvent()
    throws InvalidEventNameException
  {
    final EventEmitter events = new EventEmitter();
    final String eventName = "bar";
    final Boolean[] dataProxy = { false };
    final EventHandlerFunction eventHandler1 = (data) -> {
      dataProxy[0] = true;
    };
    assertEquals(false, dataProxy[0]);
    events.onEvent(eventName, eventHandler1);
    final String[] eventNames = events.getEventNames();
    assertEquals(1, eventNames.length);
    assertEquals(eventName, eventNames[0]);
    EventHandlerFunction[] eventHandlers;
    eventHandlers = events.getEventHandlersFor(eventName);
    assertEquals(1, eventHandlers.length);
    assertEquals(eventHandler1, eventHandlers[0]);
    events.emitEvent(eventName);
    assertEquals(true, dataProxy[0]);
    events.removeEventHandlersFor(eventName);
    final EventHandlerFunction eventHandler2 = (data) -> {
      dataProxy[0] = (Boolean) data;
    };
    assertEquals(true, dataProxy[0]);
    events.onEvent(eventName, eventHandler2);
    eventHandlers = events.getEventHandlersFor(eventName);
    assertEquals(1, eventHandlers.length);
    assertEquals(eventHandler2, eventHandlers[0]);
    events.emitEvent(eventName, false);
    assertEquals(false, dataProxy[0]);
    assertThrows(InvalidEventNameException.class, () -> {
      events.emitEvent("   ");
    });
    assertThrows(InvalidEventNameException.class, () -> {
      events.emitEvent("");
    });
    assertThrows(InvalidEventNameException.class, () -> {
      events.emitEvent(" \t ");
    });
    assertThrows(InvalidEventNameException.class, () -> {
      events.emitEvent(" \n ");
    });
  }

  @Test
  @DisplayName("EventEmitter#removeAllEventHandlers()")
  void testRemoveAllEventHandlers()
    throws InvalidEventNameException
  {
    final EventEmitter events = new EventEmitter();
    final String eventName1 = "bar";
    EventHandlerFunction[] eventHandlers;
    eventHandlers = events.getEventHandlersFor(eventName1);
    assertEquals(0, eventHandlers.length);
    final EventHandlerFunction eventHandler1 = (data) -> {};
    events.onEvent(eventName1, eventHandler1);
    final EventHandlerFunction eventHandler2 = (data) -> {};
    events.onEvent(eventName1, eventHandler2);
    eventHandlers = events.getEventHandlersFor(eventName1);
    assertEquals(2, eventHandlers.length);
    final String eventName2 = "baz";
    eventHandlers = events.getEventHandlersFor(eventName2);
    assertEquals(0, eventHandlers.length);
    final EventHandlerFunction eventHandler3 = (data) -> {};
    events.onEvent(eventName2, eventHandler3);
    eventHandlers = events.getEventHandlersFor(eventName2);
    assertEquals(1, eventHandlers.length);
    final String eventName3 = "foo";
    eventHandlers = events.getEventHandlersFor(eventName3);
    assertEquals(0, eventHandlers.length);
    final EventHandlerFunction eventHandler4 = (data) -> {};
    events.onEvent(eventName3, eventHandler4);
    final EventHandlerFunction eventHandler5 = (data) -> {};
    events.onEvent(eventName3, eventHandler5);
    final EventHandlerFunction eventHandler6 = (data) -> {};
    events.onEvent(eventName3, eventHandler6);
    eventHandlers = events.getEventHandlersFor(eventName3);
    assertEquals(3, eventHandlers.length);
    String[] eventNames;
    eventNames = events.getEventNames();
    Arrays.sort(eventNames, EventEmitterTests.enUsCollator);
    assertEquals(3, eventNames.length);
    events.removeAllEventHandlers();
    eventHandlers = events.getEventHandlersFor(eventName1);
    assertEquals(0, eventHandlers.length);
    eventHandlers = events.getEventHandlersFor(eventName2);
    assertEquals(0, eventHandlers.length);
    eventHandlers = events.getEventHandlersFor(eventName3);
    assertEquals(0, eventHandlers.length);
    eventNames = events.getEventNames();
    assertEquals(0, eventNames.length);
  }

  @Test
  @DisplayName("EventEmitter#removeEventHandlerFor(…)")
  void testRemoveEventHandlerFor()
    throws InvalidEventNameException
  {
    final EventEmitter events = new EventEmitter();
    final String eventName1 = "bar";
    EventHandlerFunction[] eventHandlers;
    eventHandlers = events.getEventHandlersFor(eventName1);
    assertEquals(0, eventHandlers.length);
    final EventHandlerFunction eventHandler1 = (data) -> {};
    events.onEvent(eventName1, eventHandler1);
    eventHandlers = events.getEventHandlersFor(eventName1);
    assertEquals(1, eventHandlers.length);
    assertEquals(eventHandler1, eventHandlers[0]);
    events.removeEventHandlerFor(eventName1, eventHandler1);
    eventHandlers = events.getEventHandlersFor(eventName1);
    assertEquals(0, eventHandlers.length);
    final String eventName2 = "baz";
    eventHandlers = events.getEventHandlersFor(eventName2);
    assertEquals(0, eventHandlers.length);
    final EventHandlerFunction eventHandler2 = (data) -> {};
    events.onEvent(eventName2, eventHandler2);
    final EventHandlerFunction eventHandler3 = (data) -> {};
    events.onEvent(eventName2, eventHandler3);
    events.onEvent(eventName2, eventHandler2);
    eventHandlers = events.getEventHandlersFor(eventName2);
    assertEquals(3, eventHandlers.length);
    assertEquals(eventHandler2, eventHandlers[0]);
    assertEquals(eventHandler3, eventHandlers[1]);
    assertEquals(eventHandler2, eventHandlers[2]);
    events.removeEventHandlerFor(eventName2, eventHandler2);
    eventHandlers = events.getEventHandlersFor(eventName2);
    assertEquals(2, eventHandlers.length);
    assertEquals(eventHandler2, eventHandlers[0]);
    assertEquals(eventHandler3, eventHandlers[1]);
    events.removeEventHandlerFor(eventName2, eventHandler2);
    eventHandlers = events.getEventHandlersFor(eventName2);
    assertEquals(1, eventHandlers.length);
    assertEquals(eventHandler3, eventHandlers[0]);
    events.removeEventHandlerFor(eventName2, eventHandler3);
    eventHandlers = events.getEventHandlersFor(eventName2);
    assertEquals(0, eventHandlers.length);
  }

  @Test
  @DisplayName("EventEmitter#removeEventHandlersFor(…)")
  void testRemoveEventHandlersFor()
    throws InvalidEventNameException
  {
    final EventEmitter events = new EventEmitter();
    final String eventName1 = "bar";
    EventHandlerFunction[] eventHandlers;
    eventHandlers = events.getEventHandlersFor(eventName1);
    assertEquals(0, eventHandlers.length);
    final EventHandlerFunction eventHandler1 = (data) -> {};
    events.onEvent(eventName1, eventHandler1);
    final EventHandlerFunction eventHandler2 = (data) -> {};
    events.onEvent(eventName1, eventHandler2);
    eventHandlers = events.getEventHandlersFor(eventName1);
    assertEquals(2, eventHandlers.length);
    events.removeEventHandlersFor(eventName1);
    eventHandlers = events.getEventHandlersFor(eventName1);
    assertEquals(0, eventHandlers.length);
    final String eventName2 = "baz";
    eventHandlers = events.getEventHandlersFor(eventName2);
    assertEquals(0, eventHandlers.length);
    final EventHandlerFunction eventHandler3 = (data) -> {};
    events.onEvent(eventName2, eventHandler3);
    eventHandlers = events.getEventHandlersFor(eventName2);
    assertEquals(1, eventHandlers.length);
    events.removeEventHandlersFor(eventName2);
    eventHandlers = events.getEventHandlersFor(eventName2);
    assertEquals(0, eventHandlers.length);
  }
}
