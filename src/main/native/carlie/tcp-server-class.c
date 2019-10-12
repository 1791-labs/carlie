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



#include <carlie/tcp-server-class.h>



void
carlie_tcp_server_handle_async_uv_close(uv_async_t * handle)
{
  assert(handle != null_ptr);
  carlie_tcp_server_async_uv_close_data_t *const data = (carlie_tcp_server_async_uv_close_data_t *) uv_handle_get_data((uv_handle_t *) handle);
  assert(data != null_ptr);
  uv_close_cb const callback = data->callback;
  uv_handle_t *const handle1 = data->handle;
  uv_close((uv_handle_t *) handle, carlie_tcp_server_handle_async_uv_close_done);
  int32_t const uv_result = (int32_t) uv_is_closing(handle1);
  if (uv_result == 0) {
    uv_close(handle1, callback);
  }
}



void
carlie_tcp_server_handle_async_uv_close_done(uv_handle_t * handle)
{
  assert(handle != null_ptr);
  carlie_tcp_server_async_uv_close_data_t *const data = (carlie_tcp_server_async_uv_close_data_t *) uv_handle_get_data(handle);
  assert(data != null_ptr);
  free(data);
  free(handle);
}



void
carlie_tcp_server_handle_async_uv_read(uv_async_t * handle)
{
  assert(handle != null_ptr);
  uv_loop_t *const loop_handle = uv_handle_get_loop((uv_handle_t *) handle);
  assert(loop_handle != null_ptr);
  carlie_tcp_server_native_object_loop_data_t *const loop_data = (carlie_tcp_server_native_object_loop_data_t *) uv_loop_get_data(loop_handle);
  assert(loop_data != null_ptr);
  jni_environment_handle_t const environment = loop_data->environment;
  assert(environment != null_ptr);
  carlie_tcp_server_async_uv_read_data_t *const data = (carlie_tcp_server_async_uv_read_data_t *) uv_handle_get_data((uv_handle_t *) handle);
  assert(data != null_ptr);
  carlie_tcp_server_connection_native_object_t *const native_object = data->native_object;
  int32_t uv_result;
  // TODO: Is this check actually necessary? Just being careful, but
  // `uv_read_start(…)` probably already handles this case and returns an
  // appropriate error. Look into this.
  uv_result = (int32_t) uv_is_closing((uv_handle_t *) native_object->tcp_handle);
  if (uv_result != 0) {
    jni_object_t const bytes_read_count_object = environment[0]->NewObject(environment, native_object->server_native_object->integer_class, native_object->server_native_object->integer_constructor_method_id, (jni_int_t) 0);
    if (bytes_read_count_object != null_ptr) {
      environment[0]->CallObjectMethod(environment, data->callback_function_object, data->callback_function_invoke_method_id, bytes_read_count_object, null_ptr);
    } else {
      carlie_throw_null_pointer_exception(environment, native_object->server_native_object->null_pointer_exception_class, native_object->server_native_object->null_pointer_exception_constructor_method_id);
    }
    environment[0]->DeleteGlobalRef(environment, data->callback_function_object);
    carlie_release_array_bytes(environment, data->buffer, data->buffer_array, (int32_t) JNI_ABORT);
    environment[0]->DeleteGlobalRef(environment, (jni_object_t) data->buffer_array);
    uv_close((uv_handle_t *) handle, carlie_tcp_server_handle_async_uv_read_done);
    return;
  }
  uv_mutex_lock(native_object->close_flag_mutex);
  native_object->latest_async_uv_read_data = data;
  uv_result = (int32_t) uv_read_start((uv_stream_t *) native_object->tcp_handle, carlie_tcp_server_handle_async_uv_read_allocate_buffer, carlie_tcp_server_handle_async_uv_read_data_read);
  if (uv_result < 0) {
    uv_mutex_unlock(native_object->close_flag_mutex);
    carlie_tcp_server_connection_emit_uv_error_event(environment, native_object, uv_result);
    jni_object_t const bytes_read_count_object = environment[0]->NewObject(environment, native_object->server_native_object->integer_class, native_object->server_native_object->integer_constructor_method_id, (jni_int_t) 0);
    if (bytes_read_count_object != null_ptr) {
      environment[0]->CallObjectMethod(environment, data->callback_function_object, data->callback_function_invoke_method_id, bytes_read_count_object, null_ptr);
    } else {
      carlie_throw_null_pointer_exception(environment, native_object->server_native_object->null_pointer_exception_class, native_object->server_native_object->null_pointer_exception_constructor_method_id);
    }
    environment[0]->DeleteGlobalRef(environment, data->callback_function_object);
    carlie_release_array_bytes(environment, data->buffer, data->buffer_array, (int32_t) JNI_ABORT);
    environment[0]->DeleteGlobalRef(environment, (jni_object_t) data->buffer_array);
    uv_close((uv_handle_t *) handle, carlie_tcp_server_handle_async_uv_read_done);
  }
}



void
carlie_tcp_server_handle_async_uv_read_allocate_buffer(uv_handle_t * handle,
                                                       size_t suggested_size,
                                                       uv_buf_t * buffer)
{
  CARLIE_INTERNAL_UNUSED_SYMBOL(suggested_size);
  assert(handle != null_ptr);
  carlie_tcp_server_connection_native_object_t *const native_object = (carlie_tcp_server_connection_native_object_t *) uv_handle_get_data(handle);
  assert(native_object != null_ptr);
  carlie_tcp_server_async_uv_read_data_t *const async_data = native_object->latest_async_uv_read_data;
  assert(async_data != null_ptr);
  buffer->base = (char *) async_data->buffer;
  buffer->len = async_data->buffer_size;
}



void
carlie_tcp_server_handle_async_uv_read_data_read(uv_stream_t * handle,
                                                 ssize_t bytes_read_count,
                                                 uv_buf_t const * buffer)
{
  CARLIE_INTERNAL_UNUSED_SYMBOL(buffer);
  assert(handle != null_ptr);
  uv_loop_t *const loop_handle = uv_handle_get_loop((uv_handle_t *) handle);
  assert(loop_handle != null_ptr);
  carlie_tcp_server_native_object_loop_data_t *const loop_data = (carlie_tcp_server_native_object_loop_data_t *) uv_loop_get_data(loop_handle);
  assert(loop_data != null_ptr);
  jni_environment_handle_t const environment = loop_data->environment;
  assert(environment != null_ptr);
  carlie_tcp_server_connection_native_object_t *const native_object = (carlie_tcp_server_connection_native_object_t *) uv_handle_get_data((uv_handle_t *) handle);
  assert(native_object != null_ptr);
  carlie_tcp_server_async_uv_read_data_t *const async_data = native_object->latest_async_uv_read_data;
  assert(async_data != null_ptr);
  if ((bytes_read_count >= 0) ||
      (bytes_read_count == UV_EOF)) {
    bytes_read_count = (bytes_read_count != UV_EOF) ?
      bytes_read_count :
      -1;
    if (bytes_read_count > 0) {
      // NOTE: This is very important in this case!
      int32_t const buffer_array_bytes_release_mode = 0;
      carlie_release_array_bytes(environment, async_data->buffer, async_data->buffer_array, buffer_array_bytes_release_mode);
    }
    jni_object_t const bytes_read_count_object = environment[0]->NewObject(environment, native_object->server_native_object->integer_class, native_object->server_native_object->integer_constructor_method_id, (jni_int_t) (int32_t) bytes_read_count);
    if (bytes_read_count_object != null_ptr) {
      environment[0]->CallObjectMethod(environment, async_data->callback_function_object, async_data->callback_function_invoke_method_id, bytes_read_count_object, null_ptr);
    } else {
      carlie_throw_null_pointer_exception(environment, native_object->server_native_object->null_pointer_exception_class, native_object->server_native_object->null_pointer_exception_constructor_method_id);
    }
  } else {
    jni_object_t exception_object = null_ptr;
    carlie_tcp_server_result_t const carlie_result = carlie_tcp_server_create_uv_exception(environment, native_object->server_native_object, bytes_read_count, &exception_object);
    if (carlie_result == CARLIE_TCP_SERVER_RESULT_SUCCESS) {
      environment[0]->CallObjectMethod(environment, async_data->callback_function_object, async_data->callback_function_invoke_method_id, null_ptr, exception_object);
    } else {
      carlie_throw_null_pointer_exception(environment, native_object->server_native_object->null_pointer_exception_class, native_object->server_native_object->null_pointer_exception_constructor_method_id);
    }
  }
  uv_mutex_unlock(native_object->close_flag_mutex);
  environment[0]->DeleteGlobalRef(environment, async_data->callback_function_object);
  if (bytes_read_count <= 0) {
    carlie_release_array_bytes(environment, async_data->buffer, async_data->buffer_array, JNI_ABORT);
  }
  environment[0]->DeleteGlobalRef(environment, (jni_object_t) async_data->buffer_array);
  uv_close((uv_handle_t *) async_data->async_handle, carlie_tcp_server_handle_async_uv_read_done);
  native_object->latest_async_uv_read_data = null_ptr;
  int32_t const uv_result = (int32_t) uv_read_stop(handle);
  if (uv_result < 0) {
    carlie_tcp_server_connection_emit_uv_error_event(environment, native_object, uv_result);
  }
}



void
carlie_tcp_server_handle_async_uv_read_done(uv_handle_t * handle)
{
  assert(handle != null_ptr);
  carlie_tcp_server_async_uv_read_data_t *const data = (carlie_tcp_server_async_uv_read_data_t *) uv_handle_get_data(handle);
  assert(data != null_ptr);
  free(data);
  free(handle);
}



void
carlie_tcp_server_handle_async_uv_server_close(uv_async_t * handle)
{
  assert(handle != null_ptr);
  uv_loop_t *const loop_handle = uv_handle_get_loop((uv_handle_t *) handle);
  assert(loop_handle != null_ptr);
  uv_close((uv_handle_t *) handle, carlie_tcp_server_handle_async_uv_server_close_done);
  carlie_tcp_server_native_object_loop_data_t *const loop_data = (carlie_tcp_server_native_object_loop_data_t *) uv_loop_get_data(loop_handle);
  assert(loop_data != null_ptr);
  carlie_tcp_server_native_object_t *const native_object = loop_data->server_native_object;
  uv_walk(loop_handle, carlie_tcp_server_handle_async_uv_server_close_walk_step, (void *) native_object);
  int32_t const uv_result = (int32_t) uv_is_closing((uv_handle_t *) native_object->tcp_handle);
  assert(uv_result == 0);
  uv_close((uv_handle_t *) native_object->tcp_handle, carlie_tcp_server_handle_uv_server_closed);
}



void
carlie_tcp_server_handle_async_uv_server_close_done(uv_handle_t * handle)
{
  assert(handle != null_ptr);
  free(handle);
}



void
carlie_tcp_server_handle_async_uv_server_close_walk_step(uv_handle_t * handle,
                                                         void * data)
{
  assert(handle != null_ptr);
  carlie_tcp_server_native_object_t *const native_object = (carlie_tcp_server_native_object_t *) data;
  if (handle == ((uv_handle_t *) native_object->tcp_handle)) return;
  int32_t const uv_result = (int32_t) uv_is_closing(handle);
  if (uv_result != 0) return;
  uv_close(handle, null_ptr);
}



void
carlie_tcp_server_handle_async_uv_write(uv_async_t * handle)
{
  assert(handle != null_ptr);
  uv_loop_t *const loop_handle = uv_handle_get_loop((uv_handle_t *) handle);
  assert(loop_handle != null_ptr);
  carlie_tcp_server_native_object_loop_data_t *const loop_data = (carlie_tcp_server_native_object_loop_data_t *) uv_loop_get_data(loop_handle);
  assert(loop_data != null_ptr);
  jni_environment_handle_t const environment = loop_data->environment;
  assert(environment != null_ptr);
  carlie_tcp_server_async_uv_write_data_t *const data = (carlie_tcp_server_async_uv_write_data_t *) uv_handle_get_data((uv_handle_t *) handle);
  assert(data != null_ptr);
  uv_buf_t *const buffer = data->buffer;
  jni_byte_array_t const buffer_array = data->buffer_array;
  jni_method_id_t const callback_function_invoke_method_id = data->callback_function_invoke_method_id;
  jni_object_t const callback_function_object = data->callback_function_object;
  carlie_tcp_server_connection_native_object_t *const native_object = data->native_object;
  uv_close((uv_handle_t *) handle, carlie_tcp_server_handle_async_uv_write_done);
  int32_t uv_result;
  // TODO: Is this check actually necessary? Just being careful, but
  // `uv_try_write(…)` probably already handles this case and returns an
  // appropriate error. Look into this.
  uv_result = (int32_t) uv_is_closing((uv_handle_t *) native_object->tcp_handle);
  if (uv_result != 0) {
    jni_object_t const bytes_written_count_object = environment[0]->NewObject(environment, native_object->server_native_object->integer_class, native_object->server_native_object->integer_constructor_method_id, (jni_int_t) 0);
    if (bytes_written_count_object != null_ptr) {
      environment[0]->CallObjectMethod(environment, data->callback_function_object, data->callback_function_invoke_method_id, bytes_written_count_object, null_ptr);
    } else {
      carlie_throw_null_pointer_exception(environment, native_object->server_native_object->null_pointer_exception_class, native_object->server_native_object->null_pointer_exception_constructor_method_id);
    }
    environment[0]->DeleteGlobalRef(environment, callback_function_object);
    carlie_release_array_bytes(environment, (uint8_t *) buffer->base, buffer_array, (int32_t) JNI_ABORT);
    environment[0]->DeleteGlobalRef(environment, (jni_object_t) buffer_array);
    free(buffer);
    return;
  }
  uv_mutex_lock(native_object->close_flag_mutex);
  uv_result = (int32_t) uv_try_write((uv_stream_t *) native_object->tcp_handle, buffer, 1u);
  if (uv_result >= 0) {
    size_t const bytes_written_count = (size_t) uv_result;
    jni_object_t const bytes_written_count_object = environment[0]->NewObject(environment, native_object->server_native_object->integer_class, native_object->server_native_object->integer_constructor_method_id, (jni_int_t) (int32_t) bytes_written_count);
    if (bytes_written_count_object != null_ptr) {
      environment[0]->CallObjectMethod(environment, callback_function_object, callback_function_invoke_method_id, bytes_written_count_object, null_ptr);
    } else {
      carlie_throw_null_pointer_exception(environment, native_object->server_native_object->null_pointer_exception_class, native_object->server_native_object->null_pointer_exception_constructor_method_id);
    }
  } else {
    if (uv_result == UV_EAGAIN) {
      carlie_tcp_server_result_t const carlie_result = carlie_tcp_server_async_uv_write(environment, native_object, buffer_array, buffer->len, callback_function_object, callback_function_invoke_method_id, &uv_result);
      if (carlie_result != CARLIE_TCP_SERVER_RESULT_SUCCESS) {
        switch (carlie_result) {
          case CARLIE_TCP_SERVER_RESULT_MEMORY_ALLOCATION_FAILED: {
            carlie_throw_runtime_exception(environment, native_object->server_native_object->runtime_exception_class, native_object->server_native_object->runtime_exception_constructor_method_id);
            break;
          }
          case CARLIE_TCP_SERVER_RESULT_UV_FAILURE: {
            carlie_tcp_server_connection_emit_uv_error_event(environment, native_object, uv_result);
            break;
          }
          default: {
            // Unreachable in this case.
            break;
          }
        }
        jni_object_t const bytes_written_count_object = environment[0]->NewObject(environment, native_object->server_native_object->integer_class, native_object->server_native_object->integer_constructor_method_id, (jni_int_t) 0);
        if (bytes_written_count_object != null_ptr) {
          environment[0]->CallObjectMethod(environment, data->callback_function_object, data->callback_function_invoke_method_id, bytes_written_count_object, null_ptr);
        } else {
          carlie_throw_null_pointer_exception(environment, native_object->server_native_object->null_pointer_exception_class, native_object->server_native_object->null_pointer_exception_constructor_method_id);
        }
      }
    } else {
      jni_object_t exception_object = null_ptr;
      carlie_tcp_server_result_t const carlie_result = carlie_tcp_server_create_uv_exception(environment, native_object->server_native_object, uv_result, &exception_object);
      if (carlie_result == CARLIE_TCP_SERVER_RESULT_SUCCESS) {
        environment[0]->CallObjectMethod(environment, callback_function_object, callback_function_invoke_method_id, null_ptr, exception_object);
      } else {
        carlie_throw_null_pointer_exception(environment, native_object->server_native_object->null_pointer_exception_class, native_object->server_native_object->null_pointer_exception_constructor_method_id);
      }
    }
  }
  uv_mutex_unlock(native_object->close_flag_mutex);
  environment[0]->DeleteGlobalRef(environment, callback_function_object);
  carlie_release_array_bytes(environment, (uint8_t *) buffer->base, buffer_array, (int32_t) JNI_ABORT);
  environment[0]->DeleteGlobalRef(environment, (jni_object_t) buffer_array);
  free(buffer);
}



void
carlie_tcp_server_handle_async_uv_write_done(uv_handle_t * handle)
{
  assert(handle != null_ptr);
  carlie_tcp_server_async_uv_write_data_t *const data = (carlie_tcp_server_async_uv_write_data_t *) uv_handle_get_data(handle);
  assert(data != null_ptr);
  free(data);
  free(handle);
}



void
carlie_tcp_server_handle_uv_connection_closed(uv_handle_t * handle)
{
  assert(handle != null_ptr);
  uv_loop_t *const loop_handle = uv_handle_get_loop(handle);
  assert(loop_handle != null_ptr);
  carlie_tcp_server_native_object_loop_data_t *const loop_data = (carlie_tcp_server_native_object_loop_data_t *) uv_loop_get_data(loop_handle);
  assert(loop_data != null_ptr);
  jni_environment_handle_t const environment = loop_data->environment;
  assert(environment != null_ptr);
  int32_t const jni_result = (int32_t) environment[0]->PushLocalFrame(environment, (jni_int_t) 1);
  // There’s no point in doing anything extra here.
  if (jni_result != 0) return;
  carlie_tcp_server_connection_native_object_t *const native_object = (carlie_tcp_server_connection_native_object_t *) uv_handle_get_data(handle);
  assert(native_object != null_ptr);
  environment[0]->CallVoidMethod(environment, native_object->handle_closed_event_function_object, native_object->handle_closed_event_function_handle_method_id);
  environment[0]->PopLocalFrame(environment, null_ptr);
}



void
carlie_tcp_server_handle_uv_connection_received(uv_stream_t * stream,
                                                int uv_connection_received_status)
{
  assert(stream != null_ptr);
  uv_tcp_t *const tcp_handle = (uv_tcp_t *) stream;
  carlie_tcp_server_native_object_t *const server_native_object = (carlie_tcp_server_native_object_t *) uv_handle_get_data((uv_handle_t *) tcp_handle);
  assert(server_native_object != null_ptr);
  carlie_tcp_server_native_object_loop_data_t *const loop_data = (carlie_tcp_server_native_object_loop_data_t *) uv_loop_get_data(server_native_object->loop_handle);
  assert(loop_data != null_ptr);
  jni_environment_handle_t const environment = loop_data->environment;
  assert(environment != null_ptr);
  if (uv_connection_received_status < 0) {
    // NOTE: Only a capacity of 1 should be needed here for the local reference
    // frame, for the exception object that’ll be created in
    // `carlie_tcp_server_emit_uv_error_event(…)`, but it’s okay to be a bit
    // generous just to be safe. This can be updated at some point if there’s
    // good reason to do so.
    int32_t const jni_result = (int32_t) environment[0]->PushLocalFrame(environment, (jni_int_t) 5);
    // There’s no point in doing anything extra here.
    if (jni_result != 0) return;
    // TODO: Should the result be checked and handled when not successful?
    carlie_tcp_server_emit_uv_error_event(environment, server_native_object, (int32_t) uv_connection_received_status);
    environment[0]->PopLocalFrame(environment, null_ptr);
    return;
  }
  int32_t const jni_result = (int32_t) environment[0]->PushLocalFrame(environment, (jni_int_t) 3);
  // There’s no point in doing anything extra here.
  if (jni_result != 0) return;
  // NOTE: This object is a local reference that must be manually released.
  jni_object_t connection_native_object_bytes = null_ptr;
  carlie_tcp_server_connection_native_object_t * connection_native_object = null_ptr;
  carlie_tcp_server_create_connection_native_object(environment, server_native_object, &connection_native_object_bytes, &connection_native_object);
  int32_t uv_result;
  uv_result = (int32_t) uv_mutex_init(connection_native_object->close_flag_mutex);
  if (uv_result != 0) {
    environment[0]->PopLocalFrame(environment, null_ptr);
    return;
  }
  // TODO: Investigate this lock acquisition implementation. A quick glance at
  // the source shows that there seems to be a possible case where this call
  // would `abort()` the whole process; not good!
  uv_mutex_lock(connection_native_object->close_flag_mutex);
  jni_object_t const connection_object = environment[0]->CallObjectMethod(environment, server_native_object->create_connection_method_function_object, server_native_object->create_connection_method_function_invoke_method_id, connection_native_object_bytes);
  if (! connection_native_object->tcp_handle_is_initialized) {
    uv_mutex_unlock(connection_native_object->close_flag_mutex);
    environment[0]->PopLocalFrame(environment, null_ptr);
    return;
  }
  uv_result = (int32_t) uv_accept((uv_stream_t *) server_native_object->tcp_handle, (uv_stream_t *) connection_native_object->tcp_handle);
  // TODO: When can this happen?
  if (uv_result < 0) {
    uv_mutex_unlock(connection_native_object->close_flag_mutex);
    environment[0]->PopLocalFrame(environment, null_ptr);
    return;
  }
  uv_mutex_unlock(connection_native_object->close_flag_mutex);
  environment[0]->CallVoidMethod(environment, server_native_object->handle_client_connected_event_function_object, server_native_object->handle_client_connected_event_function_handle_method_id, connection_object);
  environment[0]->PopLocalFrame(environment, null_ptr);
}



void
carlie_tcp_server_handle_uv_server_closed(uv_handle_t * handle)
{
  assert(handle != null_ptr);
  uv_loop_t *const loop_handle = uv_handle_get_loop(handle);
  assert(loop_handle != null_ptr);
  carlie_tcp_server_native_object_loop_data_t *const loop_data = (carlie_tcp_server_native_object_loop_data_t *) uv_loop_get_data(loop_handle);
  assert(loop_data != null_ptr);
  jni_environment_handle_t const environment = loop_data->environment;
  assert(environment != null_ptr);
  int32_t const jni_result = (int32_t) environment[0]->PushLocalFrame(environment, (jni_int_t) 1);
  // There’s no point in doing anything extra here.
  if (jni_result != 0) return;
  carlie_tcp_server_native_object_t *const native_object = (carlie_tcp_server_native_object_t *) uv_handle_get_data(handle);
  assert(native_object != null_ptr);
  environment[0]->CallVoidMethod(environment, native_object->handle_closed_event_function_object, native_object->handle_closed_event_function_handle_method_id);
  environment[0]->PopLocalFrame(environment, null_ptr);
}



void
carlie_tcp_server_uv_job_close_connection(uv_work_t * handle)
{
  assert(handle != null_ptr);
  carlie_tcp_server_connection_native_object_t *const native_object = (carlie_tcp_server_connection_native_object_t *) uv_req_get_data((uv_req_t *) handle);
  jni_environment_handle_t environment = null_ptr;
  carlie_tcp_server_result_t const carlie_result = carlie_tcp_server_get_jni_environment_from_server_native_object(native_object->server_native_object, &environment);
  if (carlie_result != CARLIE_TCP_SERVER_RESULT_SUCCESS) {
    // There’s not much that can be done here.
    return;
  }
  int32_t const jni_result = (int32_t) environment[0]->PushLocalFrame(environment, (jni_int_t) 1);
  // There’s no point in doing anything extra here.
  if (jni_result != 0) return;
  environment[0]->CallObjectMethod(environment, native_object->close_method_function_object, native_object->close_method_function_invoke_method_id);
  environment[0]->PopLocalFrame(environment, null_ptr);
}



void
carlie_tcp_server_uv_job_close_connection_completed(uv_work_t * handle,
                                                    int uv_job_completed_status)
{
  assert(handle != null_ptr);
  CARLIE_INTERNAL_UNUSED_SYMBOL(uv_job_completed_status);
  uv_req_set_data((uv_req_t *) handle, null_ptr);
  free(handle);
}



JNI_DEFINE_METHOD(jni_int_t, getConnectionNativeObjectSize)(jni_environment_handle_t environment,
                                                            jni_class_t server_class)
{
  CARLIE_INTERNAL_UNUSED_SYMBOL(environment);
  CARLIE_INTERNAL_UNUSED_SYMBOL(server_class);
  size_t const size = sizeof(carlie_tcp_server_connection_native_object_t);
  assert(((uintmax_t) size) <= ((uintmax_t) INT32_MAX));
  return (jni_int_t) (int32_t) size;
}



JNI_DEFINE_METHOD(jni_int_t, getNativeObjectSize)(jni_environment_handle_t environment,
                                                  jni_class_t server_class)
{
  CARLIE_INTERNAL_UNUSED_SYMBOL(environment);
  CARLIE_INTERNAL_UNUSED_SYMBOL(server_class);
  size_t const size = sizeof(carlie_tcp_server_native_object_t);
  assert(((uintmax_t) size) <= ((uintmax_t) INT32_MAX));
  return (jni_int_t) (int32_t) size;
}



JNI_DEFINE_METHOD(jni_boolean_t, initializeNative)(jni_environment_handle_t environment,
                                                   jni_object_t server_object,
                                                   jni_object_t native_object_bytes,
                                                   jni_object_t create_connection_native_object_static_method_function_object,
                                                   jni_class_t create_connection_native_object_static_method_function_class,
                                                   jni_object_t create_connection_method_function_object,
                                                   jni_class_t create_connection_method_function_class,
                                                   jni_object_t handle_client_connected_event_function_object,
                                                   jni_class_t handle_client_connected_event_function_class,
                                                   jni_object_t handle_closed_event_function_object,
                                                   jni_class_t handle_closed_event_function_class,
                                                   jni_object_t handle_error_occurred_event_function_object,
                                                   jni_class_t handle_error_occurred_event_function_class,
                                                   jni_class_t integer_class,
                                                   jni_class_t null_pointer_exception_class,
                                                   jni_class_t runtime_exception_class,
                                                   jni_class_t uv_exception_class)
{
  CARLIE_INTERNAL_UNUSED_SYMBOL(server_object);
  jni_java_vm_t * java_vm = null_ptr;
  int32_t const jni_result = (int32_t) environment[0]->GetJavaVM(environment, &java_vm);
  if (jni_result < 0) {
    return (jni_boolean_t) false;
  }
  uv_loop_t *const loop_handle = uv_default_loop();
  if (loop_handle == null_ptr) {
    return (jni_boolean_t) false;
  }
  jni_method_id_t const create_connection_method_function_invoke_method_id = environment[0]->GetMethodID(environment, create_connection_method_function_class, "invoke", "(Ljava/lang/Object;)Ljava/lang/Object;");
  jni_method_id_t const create_connection_native_object_static_method_function_invoke_method_id = environment[0]->GetMethodID(environment, create_connection_native_object_static_method_function_class, "invoke", "()Ljava/lang/Object;");
  jni_method_id_t const handle_client_connected_event_function_handle_method_id = environment[0]->GetMethodID(environment, handle_client_connected_event_function_class, "handle", "(Lio/seventeenninetyone/carlie/TcpServer$Connection;)V");
  jni_method_id_t const handle_closed_event_function_handle_method_id = environment[0]->GetMethodID(environment, handle_closed_event_function_class, "handle", "()V");
  jni_method_id_t const handle_error_occurred_event_function_handle_method_id = environment[0]->GetMethodID(environment, handle_error_occurred_event_function_class, "handle", "(Ljava/lang/Throwable;)V");
  jni_method_id_t const integer_constructor_method_id = environment[0]->GetMethodID(environment, integer_class, "<init>", "(I)V");
  jni_method_id_t const null_pointer_exception_constructor_method_id = environment[0]->GetMethodID(environment, null_pointer_exception_class, "<init>", "()V");
  jni_method_id_t const runtime_exception_constructor_method_id = environment[0]->GetMethodID(environment, runtime_exception_class, "<init>", "()V");
  jni_method_id_t const uv_exception_constructor_method_id = environment[0]->GetMethodID(environment, uv_exception_class, "<init>", "(I)V");
  if ((create_connection_method_function_invoke_method_id == null_ptr) ||
      (create_connection_native_object_static_method_function_invoke_method_id == null_ptr) ||
      (handle_client_connected_event_function_handle_method_id == null_ptr) ||
      (handle_closed_event_function_handle_method_id == null_ptr) ||
      (handle_error_occurred_event_function_handle_method_id == null_ptr) ||
      (integer_constructor_method_id == null_ptr) ||
      (null_pointer_exception_constructor_method_id == null_ptr) ||
      (runtime_exception_constructor_method_id == null_ptr) ||
      (uv_exception_constructor_method_id == null_ptr)) {
    return (jni_boolean_t) false;
  }
  create_connection_method_function_object = environment[0]->NewGlobalRef(environment, create_connection_method_function_object);
  create_connection_native_object_static_method_function_object = environment[0]->NewGlobalRef(environment, create_connection_native_object_static_method_function_object);
  handle_client_connected_event_function_object = environment[0]->NewGlobalRef(environment, handle_client_connected_event_function_object);
  handle_closed_event_function_object = environment[0]->NewGlobalRef(environment, handle_closed_event_function_object);
  handle_error_occurred_event_function_object = environment[0]->NewGlobalRef(environment, handle_error_occurred_event_function_object);
  integer_class = (jni_class_t) environment[0]->NewGlobalRef(environment, (jni_object_t) integer_class);
  null_pointer_exception_class = (jni_class_t) environment[0]->NewGlobalRef(environment, (jni_object_t) null_pointer_exception_class);
  runtime_exception_class = (jni_class_t) environment[0]->NewGlobalRef(environment, (jni_object_t) runtime_exception_class);
  uv_exception_class = (jni_class_t) environment[0]->NewGlobalRef(environment, (jni_object_t) uv_exception_class);
  if ((create_connection_method_function_object == null_ptr) ||
      (create_connection_native_object_static_method_function_object == null_ptr) ||
      (handle_client_connected_event_function_object == null_ptr) ||
      (handle_closed_event_function_object == null_ptr) ||
      (handle_error_occurred_event_function_object == null_ptr) ||
      (integer_class == null_ptr) ||
      (null_pointer_exception_class == null_ptr) ||
      (runtime_exception_class == null_ptr) ||
      (uv_exception_class == null_ptr)) {
    jni_object_t const global_object_references[] = {
      create_connection_method_function_object,
      create_connection_native_object_static_method_function_object,
      handle_client_connected_event_function_object,
      handle_closed_event_function_object,
      handle_error_occurred_event_function_object,
      (jni_object_t) integer_class,
      (jni_object_t) null_pointer_exception_class,
      (jni_object_t) runtime_exception_class,
      (jni_object_t) uv_exception_class,
    };
    size_t const global_object_references_count = sizeof(global_object_references) / sizeof(global_object_references[0]);
    for (size_t i = 0u; i < global_object_references_count; i++) {
      jni_object_t const global_object_reference = global_object_references[i];
      if (global_object_reference == null_ptr) continue;
      environment[0]->DeleteGlobalRef(environment, global_object_reference);
    }
    return (jni_boolean_t) false;
  }
  carlie_tcp_server_native_object_t * native_object = null_ptr;
  carlie_get_native_object(environment, native_object_bytes, (void **) &native_object);
  native_object->java_vm = java_vm;
  native_object->loop_handle = loop_handle;
  native_object->tcp_handle = &native_object->tcp_handle_;
  native_object->create_connection_method_function_invoke_method_id = create_connection_method_function_invoke_method_id;
  native_object->create_connection_method_function_object = create_connection_method_function_object;
  native_object->create_connection_native_object_static_method_function_invoke_method_id = create_connection_native_object_static_method_function_invoke_method_id;
  native_object->create_connection_native_object_static_method_function_object = create_connection_native_object_static_method_function_object;
  native_object->handle_client_connected_event_function_handle_method_id = handle_client_connected_event_function_handle_method_id;
  native_object->handle_client_connected_event_function_object = handle_client_connected_event_function_object;
  native_object->handle_closed_event_function_handle_method_id = handle_closed_event_function_handle_method_id;
  native_object->handle_closed_event_function_object = handle_closed_event_function_object;
  native_object->handle_error_occurred_event_function_handle_method_id = handle_error_occurred_event_function_handle_method_id;
  native_object->handle_error_occurred_event_function_object = handle_error_occurred_event_function_object;
  native_object->handle_uv_connection_received = carlie_tcp_server_handle_uv_connection_received;
  native_object->integer_class = integer_class;
  native_object->integer_constructor_method_id = integer_constructor_method_id;
  native_object->null_pointer_exception_class = null_pointer_exception_class;
  native_object->null_pointer_exception_constructor_method_id = null_pointer_exception_constructor_method_id;
  native_object->runtime_exception_class = runtime_exception_class;
  native_object->runtime_exception_constructor_method_id = runtime_exception_constructor_method_id;
  native_object->uv_exception_class = uv_exception_class;
  native_object->uv_exception_constructor_method_id = uv_exception_constructor_method_id;
  return (jni_boolean_t) true;
}



JNI_DEFINE_METHOD(void, closeNative)(jni_environment_handle_t environment,
                                     jni_object_t server_object,
                                     jni_object_t native_object_bytes)
{
  CARLIE_INTERNAL_UNUSED_SYMBOL(server_object);
  carlie_tcp_server_native_object_t * native_object = null_ptr;
  carlie_get_native_object(environment, native_object_bytes, (void **) &native_object);
  jni_object_t const global_object_references[] = {
    native_object->create_connection_method_function_object,
    native_object->create_connection_native_object_static_method_function_object,
    native_object->handle_client_connected_event_function_object,
    native_object->handle_closed_event_function_object,
    native_object->handle_error_occurred_event_function_object,
    (jni_object_t) native_object->integer_class,
    (jni_object_t) native_object->null_pointer_exception_class,
    (jni_object_t) native_object->runtime_exception_class,
    (jni_object_t) native_object->uv_exception_class,
  };
  size_t const global_object_references_count = sizeof(global_object_references) / sizeof(global_object_references[0]);
  for (size_t i = global_object_references_count; i > 0u;) {
    i--;
    jni_object_t const global_object_reference = global_object_references[i];
    assert(global_object_reference != null_ptr);
    environment[0]->DeleteGlobalRef(environment, global_object_reference);
  }
  uv_handle_set_data((uv_handle_t *) native_object->tcp_handle, null_ptr);
  // Zero out the native object by setting it to an empty one.
  native_object[0] = empty_carlie_tcp_server_native_object;
}



JNI_DEFINE_METHOD(void, bindUvTcpHandle)(jni_environment_handle_t environment,
                                         jni_object_t server_object,
                                         jni_object_t native_object_bytes,
                                         jni_string_t ip_address_string,
                                         jni_byte_t ip_address_type,
                                         jni_int_t port)
{
  CARLIE_INTERNAL_UNUSED_SYMBOL(server_object);
  carlie_tcp_server_native_object_t * native_object = null_ptr;
  carlie_get_native_object(environment, native_object_bytes, (void **) &native_object);
  char const *const ip_address = environment[0]->GetStringUTFChars(environment, ip_address_string, null_ptr);
  if (ip_address == null_ptr) {
    carlie_throw_null_pointer_exception(environment, native_object->null_pointer_exception_class, native_object->null_pointer_exception_constructor_method_id);
    int32_t uv_result;
    carlie_tcp_server_result_t const carlie_result = carlie_tcp_server_async_uv_close(native_object, (uv_handle_t *) native_object->tcp_handle, null_ptr, &uv_result);
    if (carlie_result != CARLIE_TCP_SERVER_RESULT_SUCCESS) {
      switch (carlie_result) {
        case CARLIE_TCP_SERVER_RESULT_MEMORY_ALLOCATION_FAILED: {
          carlie_throw_runtime_exception(environment, native_object->runtime_exception_class, native_object->runtime_exception_constructor_method_id);
          return;
        }
        case CARLIE_TCP_SERVER_RESULT_UV_FAILURE: {
          carlie_tcp_server_throw_uv_exception(environment, native_object, uv_result);
          return;
        }
        default: {
          // Unreachable in this case.
          return;
        }
      }
    }
    return;
  }
  bool const ip_is_v6 = (((uint8_t) ip_address_type) == 6u);
  struct sockaddr_in socket_address_v4;
  struct sockaddr_in6 socket_address_v6;
  assert((((int) port) >= 0) &&
         (((int) port) <= ((int) UINT16_MAX)));
  int32_t uv_result;
  if (ip_is_v6) {
    uv_result = (int32_t) uv_ip6_addr(ip_address, (int) port, &socket_address_v6);
  } else {
    uv_result = (int32_t) uv_ip4_addr(ip_address, (int) port, &socket_address_v4);
  }
  if (uv_result < 0) {
    carlie_tcp_server_throw_uv_exception(environment, native_object, uv_result);
    carlie_tcp_server_result_t const carlie_result = carlie_tcp_server_async_uv_close(native_object, (uv_handle_t *) native_object->tcp_handle, null_ptr, &uv_result);
    if (carlie_result != CARLIE_TCP_SERVER_RESULT_SUCCESS) {
      switch (carlie_result) {
        case CARLIE_TCP_SERVER_RESULT_MEMORY_ALLOCATION_FAILED: {
          carlie_throw_runtime_exception(environment, native_object->runtime_exception_class, native_object->runtime_exception_constructor_method_id);
          return;
        }
        case CARLIE_TCP_SERVER_RESULT_UV_FAILURE: {
          carlie_tcp_server_throw_uv_exception(environment, native_object, uv_result);
          return;
        }
        default: {
          // Unreachable in this case.
          return;
        }
      }
    }
    return;
  }
  struct sockaddr const *const socket_address_ptr =
    (ip_is_v6) ?
      (struct sockaddr *) &socket_address_v6 :
      (struct sockaddr *) &socket_address_v4;
  uint32_t const uv_tcp_bind_flags = 0x00000000u;
  uv_result = (int32_t) uv_tcp_bind(native_object->tcp_handle, socket_address_ptr, (unsigned int) uv_tcp_bind_flags);
  if (uv_result < 0) {
    carlie_tcp_server_throw_uv_exception(environment, native_object, uv_result);
    carlie_tcp_server_result_t const carlie_result = carlie_tcp_server_async_uv_close(native_object, (uv_handle_t *) native_object->tcp_handle, null_ptr, &uv_result);
    if (carlie_result != CARLIE_TCP_SERVER_RESULT_SUCCESS) {
      switch (carlie_result) {
        case CARLIE_TCP_SERVER_RESULT_MEMORY_ALLOCATION_FAILED: {
          carlie_throw_runtime_exception(environment, native_object->runtime_exception_class, native_object->runtime_exception_constructor_method_id);
          return;
        }
        case CARLIE_TCP_SERVER_RESULT_UV_FAILURE: {
          carlie_tcp_server_throw_uv_exception(environment, native_object, uv_result);
          return;
        }
        default: {
          // Unreachable in this case.
          return;
        }
      }
    }
    return;
  }
}



JNI_DEFINE_METHOD(void, closeUvTcpHandle)(jni_environment_handle_t environment,
                                          jni_object_t server_object,
                                          jni_object_t native_object_bytes)
{
  CARLIE_INTERNAL_UNUSED_SYMBOL(server_object);
  carlie_tcp_server_native_object_t * native_object = null_ptr;
  carlie_get_native_object(environment, native_object_bytes, (void **) &native_object);
  int32_t uv_result;
  carlie_tcp_server_result_t const carlie_result = carlie_tcp_server_async_uv_server_close(native_object, &uv_result);
  if (carlie_result != CARLIE_TCP_SERVER_RESULT_SUCCESS) {
    switch (carlie_result) {
      case CARLIE_TCP_SERVER_RESULT_MEMORY_ALLOCATION_FAILED: {
        carlie_throw_runtime_exception(environment, native_object->runtime_exception_class, native_object->runtime_exception_constructor_method_id);
        return;
      }
      case CARLIE_TCP_SERVER_RESULT_UV_FAILURE: {
        carlie_tcp_server_throw_uv_exception(environment, native_object, uv_result);
        return;
      }
      default: {
        // Unreachable in this case.
        return;
      }
    }
  }
}



JNI_DEFINE_METHOD(jni_object_t, getUvTcpBoundAddress)(jni_environment_handle_t environment,
                                                      jni_object_t server_object,
                                                      jni_object_t native_object_bytes,
                                                      jni_object_t create_address_method_function_object,
                                                      jni_class_t create_address_method_function_class)
{
  CARLIE_INTERNAL_UNUSED_SYMBOL(server_object);
  carlie_tcp_server_native_object_t * native_object = null_ptr;
  carlie_get_native_object(environment, native_object_bytes, (void **) &native_object);
  jni_object_t address_object = null_ptr;
  carlie_tcp_server_get_uv_address(environment, native_object, native_object->tcp_handle, uv_tcp_getsockname, create_address_method_function_object, create_address_method_function_class, &address_object);
  return address_object;
}



JNI_DEFINE_METHOD(void, initializeUvTcpHandle)(jni_environment_handle_t environment,
                                               jni_object_t server_object,
                                               jni_object_t native_object_bytes)
{
  CARLIE_INTERNAL_UNUSED_SYMBOL(server_object);
  carlie_tcp_server_native_object_t * native_object = null_ptr;
  carlie_get_native_object(environment, native_object_bytes, (void **) &native_object);
  int32_t uv_result;
  uv_result = (int32_t) uv_tcp_init(native_object->loop_handle, native_object->tcp_handle);
  if (uv_result < 0) {
    carlie_tcp_server_throw_uv_exception(environment, native_object, uv_result);
    carlie_tcp_server_result_t const carlie_result = carlie_tcp_server_async_uv_close(native_object, (uv_handle_t *) native_object->tcp_handle, null_ptr, &uv_result);
    if (carlie_result != CARLIE_TCP_SERVER_RESULT_SUCCESS) {
      switch (carlie_result) {
        case CARLIE_TCP_SERVER_RESULT_MEMORY_ALLOCATION_FAILED: {
          carlie_throw_runtime_exception(environment, native_object->runtime_exception_class, native_object->runtime_exception_constructor_method_id);
          return;
        }
        case CARLIE_TCP_SERVER_RESULT_UV_FAILURE: {
          carlie_tcp_server_throw_uv_exception(environment, native_object, uv_result);
          return;
        }
        default: {
          // Unreachable in this case.
          return;
        }
      }
    }
    return;
  }
  uv_handle_set_data((uv_handle_t *) native_object->tcp_handle, (void *) native_object);
}



JNI_DEFINE_METHOD(void, uvRun)(jni_environment_handle_t environment,
                               jni_object_t server_object,
                               jni_object_t native_object_bytes)
{
  CARLIE_INTERNAL_UNUSED_SYMBOL(server_object);
  carlie_tcp_server_native_object_t * native_object = null_ptr;
  carlie_get_native_object(environment, native_object_bytes, (void **) &native_object);
  carlie_tcp_server_native_object_loop_data_t *const loop_data = calloc(1u, sizeof(carlie_tcp_server_native_object_loop_data_t));
  if (loop_data == null_ptr) {
    carlie_throw_runtime_exception(environment, native_object->runtime_exception_class, native_object->runtime_exception_constructor_method_id);
    return;
  }
  // NOTE: It’s perfectly fine to save this environment in the loop, because the
  // loop runs in a single thread (the current thread).
  loop_data->environment = environment;
  loop_data->server_native_object = native_object;
  uv_loop_set_data(native_object->loop_handle, (void *) loop_data);
  // NOTE: Saving the loop on the stack since the native object should be
  // cleared by the time the loop stops running.
  uv_loop_t *const loop_handle = native_object->loop_handle;
  int32_t uv_result;
  uv_result = (int32_t) uv_run(native_object->loop_handle, UV_RUN_DEFAULT);
  assert(uv_result == 0);
  free(loop_data);
  uv_loop_set_data(loop_handle, null_ptr);
  uv_result = (int32_t) uv_loop_close(loop_handle);
  assert(uv_result != UV_EBUSY);
}



JNI_DEFINE_METHOD(void, uvTcpListen)(jni_environment_handle_t environment,
                                     jni_object_t server_object,
                                     jni_object_t native_object_bytes)
{
  CARLIE_INTERNAL_UNUSED_SYMBOL(server_object);
  carlie_tcp_server_native_object_t * native_object = null_ptr;
  carlie_get_native_object(environment, native_object_bytes, (void **) &native_object);
  // Use `listen(2)` backlog of `511`, like Redis, Apache (the web server), etc.
  uint16_t const tcp_listen_backlog = 511u;
  int32_t uv_result;
  uv_result = (int32_t) uv_listen((uv_stream_t *) native_object->tcp_handle, (int) tcp_listen_backlog, native_object->handle_uv_connection_received);
  if (uv_result < 0) {
    carlie_tcp_server_throw_uv_exception(environment, native_object, uv_result);
    carlie_tcp_server_result_t const carlie_result = carlie_tcp_server_async_uv_close(native_object, (uv_handle_t *) native_object->tcp_handle, null_ptr, &uv_result);
    if (carlie_result != CARLIE_TCP_SERVER_RESULT_SUCCESS) {
      switch (carlie_result) {
        case CARLIE_TCP_SERVER_RESULT_MEMORY_ALLOCATION_FAILED: {
          carlie_throw_runtime_exception(environment, native_object->runtime_exception_class, native_object->runtime_exception_constructor_method_id);
          return;
        }
        case CARLIE_TCP_SERVER_RESULT_UV_FAILURE: {
          carlie_tcp_server_throw_uv_exception(environment, native_object, uv_result);
          return;
        }
        default: {
          // Unreachable in this case.
          return;
        }
      }
    }
    return;
  }
}



JNI_DEFINE_CONNECTION_INTERNAL_METHOD(jni_boolean_t, initializeNative)(jni_environment_handle_t environment,
                                                                       jni_object_t connection_object,
                                                                       jni_object_t connection_native_object_bytes,
                                                                       jni_object_t server_native_object_bytes,
                                                                       jni_object_t close_method_function_object,
                                                                       jni_class_t close_method_function_class,
                                                                       jni_object_t handle_closed_event_function_object,
                                                                       jni_class_t handle_closed_event_function_class,
                                                                       jni_object_t handle_error_occurred_event_function_object,
                                                                       jni_class_t handle_error_occurred_event_function_class)
{
  CARLIE_INTERNAL_UNUSED_SYMBOL(connection_object);
  carlie_tcp_server_native_object_t * server_native_object = null_ptr;
  carlie_get_native_object(environment, server_native_object_bytes, (void **) &server_native_object);
  carlie_tcp_server_connection_native_object_t * connection_native_object = null_ptr;
  carlie_get_native_object(environment, connection_native_object_bytes, (void **) &connection_native_object);
  jni_method_id_t const close_method_function_invoke_method_id = environment[0]->GetMethodID(environment, close_method_function_class, "invoke", "()Ljava/lang/Object;");
  jni_method_id_t const handle_closed_event_function_handle_method_id = environment[0]->GetMethodID(environment, handle_closed_event_function_class, "handle", "()V");
  jni_method_id_t const handle_error_occurred_event_function_handle_method_id = environment[0]->GetMethodID(environment, handle_error_occurred_event_function_class, "handle", "(Ljava/lang/Throwable;)V");
  if ((close_method_function_invoke_method_id == null_ptr) ||
      (handle_closed_event_function_handle_method_id == null_ptr) ||
      (handle_error_occurred_event_function_handle_method_id == null_ptr)) {
    return (jni_boolean_t) false;
  }
  close_method_function_object = environment[0]->NewGlobalRef(environment, close_method_function_object);
  handle_closed_event_function_object = environment[0]->NewGlobalRef(environment, handle_closed_event_function_object);
  handle_error_occurred_event_function_object = environment[0]->NewGlobalRef(environment, handle_error_occurred_event_function_object);
  if ((close_method_function_object == null_ptr) ||
      (handle_closed_event_function_object == null_ptr) ||
      (handle_error_occurred_event_function_object == null_ptr)) {
    jni_object_t const global_object_references[] = {
      close_method_function_object,
      handle_closed_event_function_object,
      handle_error_occurred_event_function_object,
    };
    size_t const global_object_references_count = sizeof(global_object_references) / sizeof(global_object_references[0]);
    for (size_t i = 0u; i < global_object_references_count; i++) {
      jni_object_t const global_object_reference = global_object_references[i];
      if (global_object_reference == null_ptr) continue;
      environment[0]->DeleteGlobalRef(environment, global_object_reference);
    }
    return (jni_boolean_t) false;
  }
  connection_native_object->server_native_object = server_native_object;
  connection_native_object->tcp_handle = &connection_native_object->tcp_handle_;
  connection_native_object->tcp_handle_is_initialized = false;
  connection_native_object->close_method_function_invoke_method_id = close_method_function_invoke_method_id;
  connection_native_object->close_method_function_object = close_method_function_object;
  connection_native_object->handle_closed_event_function_handle_method_id = handle_closed_event_function_handle_method_id;
  connection_native_object->handle_closed_event_function_object = handle_closed_event_function_object;
  connection_native_object->handle_error_occurred_event_function_handle_method_id = handle_error_occurred_event_function_handle_method_id;
  connection_native_object->handle_error_occurred_event_function_object = handle_error_occurred_event_function_object;
  return (jni_boolean_t) true;
}



JNI_DEFINE_CONNECTION_INTERNAL_METHOD(void, closeNative)(jni_environment_handle_t environment,
                                                         jni_object_t connection_object,
                                                         jni_object_t native_object_bytes)
{
  CARLIE_INTERNAL_UNUSED_SYMBOL(connection_object);
  carlie_tcp_server_connection_native_object_t * native_object = null_ptr;
  carlie_get_native_object(environment, native_object_bytes, (void **) &native_object);
  jni_object_t const global_object_references[] = {
    native_object->close_method_function_object,
    native_object->handle_closed_event_function_object,
    native_object->handle_error_occurred_event_function_object,
  };
  size_t const global_object_references_count = sizeof(global_object_references) / sizeof(global_object_references[0]);
  for (size_t i = global_object_references_count; i > 0u;) {
    i--;
    jni_object_t const global_object_reference = global_object_references[i];
    assert(global_object_reference != null_ptr);
    environment[0]->DeleteGlobalRef(environment, global_object_reference);
  }
  uv_handle_set_data((uv_handle_t *) native_object->tcp_handle, null_ptr);
  uv_mutex_destroy(native_object->close_flag_mutex);
  // Zero out the native object by setting it to an empty one.
  native_object[0] = empty_carlie_tcp_server_connection_native_object;
}



JNI_DEFINE_CONNECTION_INTERNAL_METHOD(void, closeInUvWorker)(jni_environment_handle_t environment,
                                                             jni_object_t connection_object,
                                                             jni_object_t native_object_bytes)
{
  CARLIE_INTERNAL_UNUSED_SYMBOL(connection_object);
  carlie_tcp_server_connection_native_object_t * native_object = null_ptr;
  carlie_get_native_object(environment, native_object_bytes, (void **) &native_object);
  uv_work_t *const job_handle = calloc(1u, sizeof(uv_work_t));
  if (job_handle == null_ptr) {
    carlie_throw_runtime_exception(environment, native_object->server_native_object->runtime_exception_class, native_object->server_native_object->runtime_exception_constructor_method_id);
    return;
  }
  uv_req_set_data((uv_req_t *) job_handle, (void *) native_object);
  int32_t const uv_result = (int32_t) uv_queue_work(native_object->server_native_object->loop_handle, job_handle, carlie_tcp_server_uv_job_close_connection, carlie_tcp_server_uv_job_close_connection_completed);
  if (uv_result < 0) {
    carlie_tcp_server_connection_emit_uv_error_event(environment, native_object, uv_result);
    return;
  }
}



JNI_DEFINE_CONNECTION_INTERNAL_METHOD(void, closeUvTcpHandle)(jni_environment_handle_t environment,
                                                              jni_object_t connection_object,
                                                              jni_object_t native_object_bytes)
{
  CARLIE_INTERNAL_UNUSED_SYMBOL(connection_object);
  carlie_tcp_server_connection_native_object_t * native_object = null_ptr;
  carlie_get_native_object(environment, native_object_bytes, (void **) &native_object);
  int32_t uv_result;
  carlie_tcp_server_result_t const carlie_result = carlie_tcp_server_async_uv_close(native_object->server_native_object, (uv_handle_t *) native_object->tcp_handle, carlie_tcp_server_handle_uv_connection_closed, &uv_result);
  if (carlie_result != CARLIE_TCP_SERVER_RESULT_SUCCESS) {
    switch (carlie_result) {
      case CARLIE_TCP_SERVER_RESULT_MEMORY_ALLOCATION_FAILED: {
        carlie_throw_runtime_exception(environment, native_object->server_native_object->runtime_exception_class, native_object->server_native_object->runtime_exception_constructor_method_id);
        return;
      }
      case CARLIE_TCP_SERVER_RESULT_UV_FAILURE: {
        carlie_tcp_server_throw_uv_exception(environment, native_object->server_native_object, uv_result);
        return;
      }
      default: {
        // Unreachable in this case.
        return;
      }
    }
  }
}



JNI_DEFINE_CONNECTION_INTERNAL_METHOD(jni_object_t, getUvTcpBoundAddress)(jni_environment_handle_t environment,
                                                                          jni_object_t connection_object,
                                                                          jni_object_t native_object_bytes,
                                                                          jni_object_t create_address_method_function_object,
                                                                          jni_class_t create_address_method_function_class)
{
  CARLIE_INTERNAL_UNUSED_SYMBOL(connection_object);
  carlie_tcp_server_connection_native_object_t * native_object = null_ptr;
  carlie_get_native_object(environment, native_object_bytes, (void **) &native_object);
  jni_object_t address_object = null_ptr;
  carlie_tcp_server_get_uv_address(environment, native_object->server_native_object, native_object->tcp_handle, uv_tcp_getsockname, create_address_method_function_object, create_address_method_function_class, &address_object);
  return address_object;
}



JNI_DEFINE_CONNECTION_INTERNAL_METHOD(jni_object_t, getUvTcpRemoteAddress)(jni_environment_handle_t environment,
                                                                           jni_object_t connection_object,
                                                                           jni_object_t native_object_bytes,
                                                                           jni_object_t create_address_method_function_object,
                                                                           jni_class_t create_address_method_function_class)
{
  CARLIE_INTERNAL_UNUSED_SYMBOL(connection_object);
  carlie_tcp_server_connection_native_object_t * native_object = null_ptr;
  carlie_get_native_object(environment, native_object_bytes, (void **) &native_object);
  jni_object_t address_object = null_ptr;
  carlie_tcp_server_get_uv_address(environment, native_object->server_native_object, native_object->tcp_handle, uv_tcp_getpeername, create_address_method_function_object, create_address_method_function_class, &address_object);
  return address_object;
}



JNI_DEFINE_CONNECTION_INTERNAL_METHOD(void, initializeUvTcpHandle)(jni_environment_handle_t environment,
                                                                   jni_object_t connection_object,
                                                                   jni_object_t native_object_bytes)
{
  CARLIE_INTERNAL_UNUSED_SYMBOL(connection_object);
  carlie_tcp_server_connection_native_object_t * native_object = null_ptr;
  carlie_get_native_object(environment, native_object_bytes, (void **) &native_object);
  int32_t uv_result;
  uv_result = (int32_t) uv_tcp_init(native_object->server_native_object->loop_handle, native_object->tcp_handle);
  if (uv_result < 0) {
    carlie_tcp_server_throw_uv_exception(environment, native_object->server_native_object, uv_result);
    carlie_tcp_server_result_t const carlie_result = carlie_tcp_server_async_uv_close(native_object->server_native_object, (uv_handle_t *) native_object->tcp_handle, null_ptr, &uv_result);
    if (carlie_result != CARLIE_TCP_SERVER_RESULT_SUCCESS) {
      switch (carlie_result) {
        case CARLIE_TCP_SERVER_RESULT_MEMORY_ALLOCATION_FAILED: {
          carlie_throw_runtime_exception(environment, native_object->server_native_object->runtime_exception_class, native_object->server_native_object->runtime_exception_constructor_method_id);
          return;
        }
        case CARLIE_TCP_SERVER_RESULT_UV_FAILURE: {
          carlie_tcp_server_throw_uv_exception(environment, native_object->server_native_object, uv_result);
          return;
        }
        default: {
          // Unreachable in this case.
          return;
        }
      }
    }
    return;
  }
  uv_handle_set_data((uv_handle_t *) native_object->tcp_handle, (void *) native_object);
  native_object->tcp_handle_is_initialized = true;
}



JNI_DEFINE_CONNECTION_INTERNAL_METHOD(jni_boolean_t, isCloseable)(jni_environment_handle_t environment,
                                                                  jni_object_t connection_object,
                                                                  jni_object_t native_object_bytes)
{
  CARLIE_INTERNAL_UNUSED_SYMBOL(connection_object);
  carlie_tcp_server_connection_native_object_t * native_object = null_ptr;
  carlie_get_native_object(environment, native_object_bytes, (void **) &native_object);
  // TODO: Investigate this lock acquisition implementation. A quick glance at
  // the source shows that there seems to be a possible case where this call
  // would `abort()` the whole process; not good!
  int32_t const uv_result = (int32_t) uv_mutex_trylock(native_object->close_flag_mutex);
  if (uv_result != 0) {
    return (jni_boolean_t) false;
  }
  uv_mutex_unlock(native_object->close_flag_mutex);
  return (jni_boolean_t) true;
}



JNI_DEFINE_CONNECTION_INTERNAL_METHOD(void, uvTcpDisableKeepAlive)(jni_environment_handle_t environment,
                                                                   jni_object_t connection_object,
                                                                   jni_object_t native_object_bytes)
{
  CARLIE_INTERNAL_UNUSED_SYMBOL(connection_object);
  carlie_tcp_server_connection_native_object_t * native_object = null_ptr;
  carlie_get_native_object(environment, native_object_bytes, (void **) &native_object);
  int32_t const uv_result = (int32_t) uv_tcp_keepalive(native_object->tcp_handle, (int) false, 0u);
  if (uv_result < 0) {
    carlie_tcp_server_throw_uv_exception(environment, native_object->server_native_object, uv_result);
  }
}



JNI_DEFINE_CONNECTION_INTERNAL_METHOD(void, uvTcpEnableKeepAlive)(jni_environment_handle_t environment,
                                                                  jni_object_t connection_object,
                                                                  jni_object_t native_object_bytes,
                                                                  jni_int_t initial_delay)
{
  CARLIE_INTERNAL_UNUSED_SYMBOL(connection_object);
  carlie_tcp_server_connection_native_object_t * native_object = null_ptr;
  carlie_get_native_object(environment, native_object_bytes, (void **) &native_object);
  int32_t const uv_result = (int32_t) uv_tcp_keepalive(native_object->tcp_handle, (int) true, (unsigned int) initial_delay);
  if (uv_result < 0) {
    carlie_tcp_server_throw_uv_exception(environment, native_object->server_native_object, uv_result);
  }
}



JNI_DEFINE_CONNECTION_INTERNAL_METHOD(void, uvTcpRead)(jni_environment_handle_t environment,
                                                       jni_object_t connection_object,
                                                       jni_object_t native_object_bytes,
                                                       jni_byte_array_t buffer_bytes,
                                                       jni_int_t buffer_bytes_size,
                                                       jni_object_t callback_function_object,
                                                       jni_class_t callback_function_class)
{
  CARLIE_INTERNAL_UNUSED_SYMBOL(connection_object);
  carlie_tcp_server_connection_native_object_t * native_object = null_ptr;
  carlie_get_native_object(environment, native_object_bytes, (void **) &native_object);
  jni_method_id_t const callback_function_invoke_method_id = environment[0]->GetMethodID(environment, callback_function_class, "invoke", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
  if (callback_function_invoke_method_id == null_ptr) {
    carlie_throw_null_pointer_exception(environment, native_object->server_native_object->null_pointer_exception_class, native_object->server_native_object->null_pointer_exception_constructor_method_id);
    return;
  }
  int32_t uv_result;
  carlie_tcp_server_result_t const carlie_result = carlie_tcp_server_async_uv_read(environment, native_object, buffer_bytes, (size_t) (int32_t) buffer_bytes_size, callback_function_object, callback_function_invoke_method_id, &uv_result);
  if (carlie_result != CARLIE_TCP_SERVER_RESULT_SUCCESS) {
    switch (carlie_result) {
      case CARLIE_TCP_SERVER_RESULT_MEMORY_ALLOCATION_FAILED: {
        carlie_throw_runtime_exception(environment, native_object->server_native_object->runtime_exception_class, native_object->server_native_object->runtime_exception_constructor_method_id);
        return;
      }
      case CARLIE_TCP_SERVER_RESULT_UV_FAILURE: {
        carlie_tcp_server_throw_uv_exception(environment, native_object->server_native_object, uv_result);
        return;
      }
      default: {
        // Unreachable in this case.
        return;
      }
    }
  }
}



JNI_DEFINE_CONNECTION_INTERNAL_METHOD(void, uvTcpWrite)(jni_environment_handle_t environment,
                                                        jni_object_t connection_object,
                                                        jni_object_t native_object_bytes,
                                                        jni_byte_array_t buffer_bytes,
                                                        jni_int_t buffer_bytes_size,
                                                        jni_object_t callback_function_object,
                                                        jni_class_t callback_function_class)
{
  CARLIE_INTERNAL_UNUSED_SYMBOL(connection_object);
  carlie_tcp_server_connection_native_object_t * native_object = null_ptr;
  carlie_get_native_object(environment, native_object_bytes, (void **) &native_object);
  jni_method_id_t const callback_function_invoke_method_id = environment[0]->GetMethodID(environment, callback_function_class, "invoke", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
  if (callback_function_invoke_method_id == null_ptr) {
    carlie_throw_null_pointer_exception(environment, native_object->server_native_object->null_pointer_exception_class, native_object->server_native_object->null_pointer_exception_constructor_method_id);
    return;
  }
  int32_t uv_result;
  carlie_tcp_server_result_t const carlie_result = carlie_tcp_server_async_uv_write(environment, native_object, buffer_bytes, (size_t) (int32_t) buffer_bytes_size, callback_function_object, callback_function_invoke_method_id, &uv_result);
  if (carlie_result != CARLIE_TCP_SERVER_RESULT_SUCCESS) {
    switch (carlie_result) {
      case CARLIE_TCP_SERVER_RESULT_MEMORY_ALLOCATION_FAILED: {
        carlie_throw_runtime_exception(environment, native_object->server_native_object->runtime_exception_class, native_object->server_native_object->runtime_exception_constructor_method_id);
        return;
      }
      case CARLIE_TCP_SERVER_RESULT_UV_FAILURE: {
        carlie_tcp_server_throw_uv_exception(environment, native_object->server_native_object, uv_result);
        return;
      }
      default: {
        // Unreachable in this case.
        return;
      }
    }
  }
}
