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



#ifndef IO_SEVENTEENNINETYONE_CARLIE_TCP_SERVER_CLASS_H
#define IO_SEVENTEENNINETYONE_CARLIE_TCP_SERVER_CLASS_H 1



#include <carlie/common.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <uv.h>



/*
 *******************************************************************************
 * JVM and JNI-related macros.                                                 *
 *******************************************************************************
 */
#define JVM_PACKAGE_SIGNATURE JVM_ROOT_PACKAGE_SIGNATURE
#define JVM_CLASS_NAME TcpServer
#define JNI_DEFINE_METHOD(return_type, method_signature)                                   \
  JNI_DEFINE_METHOD0(return_type, JVM_PACKAGE_SIGNATURE, JVM_CLASS_NAME, method_signature)
#define JVM_CLASS_CONNECTION_INTERNAL_NAME TcpServer_00024ConnectionInternal
#define JNI_DEFINE_CONNECTION_INTERNAL_METHOD(return_type, method_signature)                                   \
  JNI_DEFINE_METHOD0(return_type, JVM_PACKAGE_SIGNATURE, JVM_CLASS_CONNECTION_INTERNAL_NAME, method_signature)



typedef struct _carlie_tcp_server_async_uv_close_data carlie_tcp_server_async_uv_close_data_t;
typedef struct _carlie_tcp_server_async_uv_read_data carlie_tcp_server_async_uv_read_data_t;
typedef struct _carlie_tcp_server_async_uv_write_data carlie_tcp_server_async_uv_write_data_t;
typedef struct _carlie_tcp_server_connection_native_object carlie_tcp_server_connection_native_object_t;
typedef struct _carlie_tcp_server_native_object carlie_tcp_server_native_object_t;
typedef struct _carlie_tcp_server_native_object_loop_data carlie_tcp_server_native_object_loop_data_t;



typedef enum {
  CARLIE_TCP_SERVER_RESULT_SUCCESS = (unsigned int) CARLIE_RESULT_SUCCESS,
  CARLIE_TCP_SERVER_RESULT_CANNOT_CREATE_EXCEPTION = (unsigned int) CARLIE_RESULT_CANNOT_CREATE_EXCEPTION,
  CARLIE_TCP_SERVER_RESULT_CANNOT_THROW_EXCEPTION = (unsigned int) CARLIE_RESULT_CANNOT_THROW_EXCEPTION,
  CARLIE_TCP_SERVER_RESULT_NULL_POINTER_EXCEPTION_THROWN = (unsigned int) CARLIE_RESULT_NULL_POINTER_EXCEPTION_THROWN,
  CARLIE_TCP_SERVER_RESULT_RUNTIME_EXCEPTION_THROWN = (unsigned int) CARLIE_RESULT_RUNTIME_EXCEPTION_THROWN,
  CARLIE_TCP_SERVER_RESULT_CANNOT_RETRIEVE_JNI_ENVIRONMENT,
  CARLIE_TCP_SERVER_RESULT_MEMORY_ALLOCATION_FAILED,
  CARLIE_TCP_SERVER_RESULT_UV_FAILURE,
} carlie_tcp_server_result_t;



struct _carlie_tcp_server_async_uv_close_data {
  uv_close_cb callback;
  uv_handle_t * handle;
};



struct _carlie_tcp_server_async_uv_read_data {
  uv_async_t * async_handle;
  uint8_t * buffer;
  jni_byte_array_t buffer_array;
  size_t buffer_size;
  jni_method_id_t callback_function_invoke_method_id;
  jni_object_t callback_function_object;
  carlie_tcp_server_connection_native_object_t * native_object;
};



struct _carlie_tcp_server_async_uv_write_data {
  uv_buf_t * buffer;
  jni_byte_array_t buffer_array;
  jni_method_id_t callback_function_invoke_method_id;
  jni_object_t callback_function_object;
  carlie_tcp_server_connection_native_object_t * native_object;
};



struct _carlie_tcp_server_connection_native_object {
  uv_mutex_t * close_flag_mutex;
  uv_mutex_t close_flag_mutex_;
  jni_method_id_t close_method_function_invoke_method_id;
  jni_object_t close_method_function_object;
  jni_method_id_t handle_closed_event_function_handle_method_id;
  jni_object_t handle_closed_event_function_object;
  jni_method_id_t handle_error_occurred_event_function_handle_method_id;
  jni_object_t handle_error_occurred_event_function_object;
  carlie_tcp_server_async_uv_read_data_t * latest_async_uv_read_data;
  carlie_tcp_server_native_object_t * server_native_object;
  uv_tcp_t * tcp_handle;
  uv_tcp_t tcp_handle_;
  bool tcp_handle_is_initialized;
};



struct _carlie_tcp_server_native_object {
  jni_method_id_t create_connection_method_function_invoke_method_id;
  jni_object_t create_connection_method_function_object;
  jni_method_id_t create_connection_native_object_static_method_function_invoke_method_id;
  jni_object_t create_connection_native_object_static_method_function_object;
  jni_method_id_t handle_client_connected_event_function_handle_method_id;
  jni_object_t handle_client_connected_event_function_object;
  jni_method_id_t handle_closed_event_function_handle_method_id;
  jni_object_t handle_closed_event_function_object;
  jni_method_id_t handle_error_occurred_event_function_handle_method_id;
  jni_object_t handle_error_occurred_event_function_object;
  uv_connection_cb handle_uv_connection_received;
  jni_class_t integer_class;
  jni_method_id_t integer_constructor_method_id;
  jni_java_vm_t * java_vm;
  uv_loop_t * loop_handle;
  jni_class_t null_pointer_exception_class;
  jni_method_id_t null_pointer_exception_constructor_method_id;
  jni_class_t runtime_exception_class;
  jni_method_id_t runtime_exception_constructor_method_id;
  uv_tcp_t * tcp_handle;
  uv_tcp_t tcp_handle_;
  jni_class_t uv_exception_class;
  jni_method_id_t uv_exception_constructor_method_id;
};



struct _carlie_tcp_server_native_object_loop_data {
  jni_environment_handle_t environment;
  carlie_tcp_server_native_object_t * server_native_object;
};



static carlie_tcp_server_connection_native_object_t const empty_carlie_tcp_server_connection_native_object;
static carlie_tcp_server_native_object_t const empty_carlie_tcp_server_native_object;



CARLIE_C_ALWAYS_INLINE static inline carlie_tcp_server_result_t
carlie_tcp_server_async_uv_close(carlie_tcp_server_native_object_t *const native_object,
                                 uv_handle_t *const handle,
                                 uv_close_cb const callback,
                                 int32_t *const uv_result_ptr);



CARLIE_C_ALWAYS_INLINE static inline carlie_tcp_server_result_t
carlie_tcp_server_async_uv_read(jni_environment_handle_t const environment,
                                carlie_tcp_server_connection_native_object_t *const native_object,
                                jni_byte_array_t buffer_bytes,
                                size_t const buffer_bytes_size,
                                jni_object_t callback_function_object,
                                jni_method_id_t const callback_function_invoke_method_id,
                                int32_t *const uv_result_ptr);



CARLIE_C_ALWAYS_INLINE static inline carlie_tcp_server_result_t
carlie_tcp_server_async_uv_server_close(carlie_tcp_server_native_object_t *const native_object,
                                        int32_t *const uv_result_ptr);



CARLIE_C_ALWAYS_INLINE static inline carlie_tcp_server_result_t
carlie_tcp_server_async_uv_write(jni_environment_handle_t const environment,
                                 carlie_tcp_server_connection_native_object_t *const native_object,
                                 jni_byte_array_t buffer_bytes,
                                 size_t const buffer_bytes_size,
                                 jni_object_t callback_function_object,
                                 jni_method_id_t const callback_function_invoke_method_id,
                                 int32_t *const uv_result_ptr);



CARLIE_C_ALWAYS_INLINE static inline carlie_tcp_server_result_t
carlie_tcp_server_connection_emit_uv_error_event(jni_environment_handle_t const environment,
                                                 carlie_tcp_server_connection_native_object_t *const native_object,
                                                 int32_t const error_number);



CARLIE_C_ALWAYS_INLINE static inline carlie_tcp_server_result_t
carlie_tcp_server_create_connection_native_object(jni_environment_handle_t const environment,
                                                  carlie_tcp_server_native_object_t *const server_native_object,
                                                  jni_object_t *const connection_native_object_bytes_ptr,
                                                  carlie_tcp_server_connection_native_object_t **const connection_native_object_ptr);



CARLIE_C_ALWAYS_INLINE static inline carlie_tcp_server_result_t
carlie_tcp_server_create_uv_exception(jni_environment_handle_t const environment,
                                      carlie_tcp_server_native_object_t *const native_object,
                                      int32_t const error_number,
                                      jni_object_t *const exception_object_ptr);



CARLIE_C_ALWAYS_INLINE static inline carlie_tcp_server_result_t
carlie_tcp_server_emit_uv_error_event(jni_environment_handle_t const environment,
                                      carlie_tcp_server_native_object_t *const native_object,
                                      int32_t const error_number);



CARLIE_C_ALWAYS_INLINE static inline carlie_tcp_server_result_t
carlie_tcp_server_get_jni_environment_from_server_native_object(carlie_tcp_server_native_object_t *const native_object,
                                                                jni_environment_handle_t *const environment_ptr);



CARLIE_C_ALWAYS_INLINE static inline carlie_tcp_server_result_t
carlie_tcp_server_get_uv_address(jni_environment_handle_t const environment,
                                 carlie_tcp_server_native_object_t *const native_object,
                                 uv_tcp_t *const tcp_handle,
                                 int (*const uv_get_address_function)(uv_tcp_t const *const tcp_handle,
                                                                      struct sockaddr *const socket_address_ptr,
                                                                      int *const socket_address_size_ptr),
                                 jni_object_t const create_address_method_function_object,
                                 jni_class_t const create_address_method_function_class,
                                 jni_object_t *const address_object_ptr);



CARLIE_C_ALWAYS_INLINE static inline char const *
carlie_tcp_server_result_get_value(carlie_tcp_server_result_t const status);



CARLIE_C_ALWAYS_INLINE static inline carlie_tcp_server_result_t
carlie_tcp_server_throw_uv_exception(jni_environment_handle_t const environment,
                                     carlie_tcp_server_native_object_t *const native_object,
                                     int32_t const error_number);



void
carlie_tcp_server_handle_async_uv_close(uv_async_t * handle);



void
carlie_tcp_server_handle_async_uv_close_done(uv_handle_t * handle);



void
carlie_tcp_server_handle_async_uv_read(uv_async_t * handle);



void
carlie_tcp_server_handle_async_uv_read_allocate_buffer(uv_handle_t * handle,
                                                       size_t suggested_size,
                                                       uv_buf_t * buffer);



void
carlie_tcp_server_handle_async_uv_read_data_read(uv_stream_t * handle,
                                                 ssize_t bytes_read_count,
                                                 uv_buf_t const * buffer);



void
carlie_tcp_server_handle_async_uv_read_done(uv_handle_t * handle);



void
carlie_tcp_server_handle_async_uv_server_close(uv_async_t * handle);



void
carlie_tcp_server_handle_async_uv_server_close_done(uv_handle_t * handle);



void
carlie_tcp_server_handle_async_uv_server_close_walk_step(uv_handle_t * handle,
                                                         void * data);



void
carlie_tcp_server_handle_async_uv_write(uv_async_t * handle);



void
carlie_tcp_server_handle_async_uv_write_done(uv_handle_t * handle);



void
carlie_tcp_server_handle_uv_connection_closed(uv_handle_t * handle);



void
carlie_tcp_server_handle_uv_connection_received(uv_stream_t * stream,
                                                int uv_connection_received_status);



void
carlie_tcp_server_handle_uv_server_closed(uv_handle_t * handle);



void
carlie_tcp_server_uv_job_close_connection(uv_work_t * handle);



void
carlie_tcp_server_uv_job_close_connection_completed(uv_work_t * handle,
                                                    int uv_job_completed_status);



CARLIE_C_ALWAYS_INLINE static inline carlie_tcp_server_result_t
carlie_tcp_server_async_uv_close(carlie_tcp_server_native_object_t *const native_object,
                                 uv_handle_t *const handle,
                                 uv_close_cb const callback,
                                 int32_t *const uv_result_ptr)
{
  uv_async_t *const async_close_handle = calloc(1u, sizeof(uv_async_t));
  if (async_close_handle == null_ptr) {
    uv_result_ptr[0] = 0;
    return CARLIE_TCP_SERVER_RESULT_MEMORY_ALLOCATION_FAILED;
  }
  int32_t uv_result;
  uv_result = (int32_t) uv_async_init(native_object->loop_handle, async_close_handle, carlie_tcp_server_handle_async_uv_close);
  if (uv_result < 0) {
    uv_result_ptr[0] = uv_result;
    free(async_close_handle);
    return CARLIE_TCP_SERVER_RESULT_UV_FAILURE;
  }
  carlie_tcp_server_async_uv_close_data_t *const async_close_data = malloc(sizeof(carlie_tcp_server_async_uv_close_data_t));
  if (async_close_data == null_ptr) {
    uv_result_ptr[0] = 0;
    free(async_close_handle);
    return CARLIE_TCP_SERVER_RESULT_MEMORY_ALLOCATION_FAILED;
  }
  async_close_data->handle = handle;
  async_close_data->callback = callback;
  uv_handle_set_data((uv_handle_t *) async_close_handle, (void *) async_close_data);
  uv_result = (int32_t) uv_async_send(async_close_handle);
  if (uv_result < 0) {
    uv_result_ptr[0] = uv_result;
    free(async_close_data);
    free(async_close_handle);
    return CARLIE_TCP_SERVER_RESULT_UV_FAILURE;
  }
  uv_result_ptr[0] = 0;
  return CARLIE_TCP_SERVER_RESULT_SUCCESS;
}



CARLIE_C_ALWAYS_INLINE static inline carlie_tcp_server_result_t
carlie_tcp_server_async_uv_read(jni_environment_handle_t const environment,
                                carlie_tcp_server_connection_native_object_t *const native_object,
                                jni_byte_array_t buffer_bytes,
                                size_t const buffer_bytes_size,
                                jni_object_t callback_function_object,
                                jni_method_id_t const callback_function_invoke_method_id,
                                int32_t *const uv_result_ptr)
{
  uv_async_t *const async_read_handle = calloc(1u, sizeof(uv_async_t));
  if (async_read_handle == null_ptr) {
    uv_result_ptr[0] = 0;
    return CARLIE_TCP_SERVER_RESULT_MEMORY_ALLOCATION_FAILED;
  }
  int32_t uv_result;
  uv_result = (int32_t) uv_async_init(native_object->server_native_object->loop_handle, async_read_handle, carlie_tcp_server_handle_async_uv_read);
  if (uv_result < 0) {
    uv_result_ptr[0] = uv_result;
    free(async_read_handle);
    return CARLIE_TCP_SERVER_RESULT_UV_FAILURE;
  }
  carlie_tcp_server_async_uv_read_data_t *const async_read_data = malloc(sizeof(carlie_tcp_server_async_uv_read_data_t));
  if (async_read_data == null_ptr) {
    uv_result_ptr[0] = 0;
    free(async_read_handle);
    return CARLIE_TCP_SERVER_RESULT_MEMORY_ALLOCATION_FAILED;
  }
  uint8_t * buffer;
  carlie_get_array_bytes(environment, buffer_bytes, &buffer);
  buffer_bytes = (jni_byte_array_t) environment[0]->NewGlobalRef(environment, (jni_object_t) buffer_bytes);
  if (buffer_bytes == null_ptr) {
    carlie_throw_null_pointer_exception(environment, native_object->server_native_object->null_pointer_exception_class, native_object->server_native_object->null_pointer_exception_constructor_method_id);
    uv_result_ptr[0] = 0;
    carlie_release_array_bytes(environment, buffer, buffer_bytes, (int32_t) JNI_ABORT);
    free(async_read_data);
    free(async_read_handle);
    return CARLIE_TCP_SERVER_RESULT_NULL_POINTER_EXCEPTION_THROWN;
  }
  callback_function_object = environment[0]->NewGlobalRef(environment, callback_function_object);
  if (callback_function_object == null_ptr) {
    carlie_throw_null_pointer_exception(environment, native_object->server_native_object->null_pointer_exception_class, native_object->server_native_object->null_pointer_exception_constructor_method_id);
    uv_result_ptr[0] = 0;
    carlie_release_array_bytes(environment, buffer, buffer_bytes, (int32_t) JNI_ABORT);
    environment[0]->DeleteGlobalRef(environment, (jni_object_t) buffer_bytes);
    free(async_read_data);
    free(async_read_handle);
    return CARLIE_TCP_SERVER_RESULT_NULL_POINTER_EXCEPTION_THROWN;
  }
  async_read_data->async_handle = async_read_handle;
  async_read_data->buffer = buffer;
  async_read_data->buffer_array = buffer_bytes;
  async_read_data->buffer_size = buffer_bytes_size;
  async_read_data->callback_function_invoke_method_id = callback_function_invoke_method_id;
  async_read_data->callback_function_object = callback_function_object;
  async_read_data->native_object = native_object;
  uv_handle_set_data((uv_handle_t *) async_read_handle, (void *) async_read_data);
  uv_result = (int32_t) uv_async_send(async_read_handle);
  if (uv_result < 0) {
    uv_result_ptr[0] = uv_result;
    environment[0]->DeleteGlobalRef(environment, callback_function_object);
    carlie_release_array_bytes(environment, buffer, buffer_bytes, (int32_t) JNI_ABORT);
    environment[0]->DeleteGlobalRef(environment, (jni_object_t) buffer_bytes);
    free(async_read_data);
    free(async_read_handle);
    return CARLIE_TCP_SERVER_RESULT_UV_FAILURE;
  }
  uv_result_ptr[0] = 0;
  return CARLIE_TCP_SERVER_RESULT_SUCCESS;
}



CARLIE_C_ALWAYS_INLINE static inline carlie_tcp_server_result_t
carlie_tcp_server_async_uv_server_close(carlie_tcp_server_native_object_t *const native_object,
                                        int32_t *const uv_result_ptr)
{
  uv_async_t *const async_server_close_handle = calloc(1u, sizeof(uv_async_t));
  if (async_server_close_handle == null_ptr) {
    uv_result_ptr[0] = 0;
    return CARLIE_TCP_SERVER_RESULT_MEMORY_ALLOCATION_FAILED;
  }
  int32_t uv_result;
  uv_result = (int32_t) uv_async_init(native_object->loop_handle, async_server_close_handle, carlie_tcp_server_handle_async_uv_server_close);
  if (uv_result < 0) {
    uv_result_ptr[0] = uv_result;
    free(async_server_close_handle);
    return CARLIE_TCP_SERVER_RESULT_UV_FAILURE;
  }
  uv_result = (int32_t) uv_async_send(async_server_close_handle);
  if (uv_result < 0) {
    uv_result_ptr[0] = uv_result;
    free(async_server_close_handle);
    return CARLIE_TCP_SERVER_RESULT_UV_FAILURE;
  }
  return CARLIE_TCP_SERVER_RESULT_SUCCESS;
}



CARLIE_C_ALWAYS_INLINE static inline carlie_tcp_server_result_t
carlie_tcp_server_async_uv_write(jni_environment_handle_t const environment,
                                 carlie_tcp_server_connection_native_object_t *const native_object,
                                 jni_byte_array_t buffer_bytes,
                                 size_t const buffer_bytes_size,
                                 jni_object_t callback_function_object,
                                 jni_method_id_t const callback_function_invoke_method_id,
                                 int32_t *const uv_result_ptr)
{
  uv_async_t *const async_write_handle = calloc(1u, sizeof(uv_async_t));
  if (async_write_handle == null_ptr) {
    uv_result_ptr[0] = 0;
    return CARLIE_TCP_SERVER_RESULT_MEMORY_ALLOCATION_FAILED;
  }
  int32_t uv_result;
  uv_result = (int32_t) uv_async_init(native_object->server_native_object->loop_handle, async_write_handle, carlie_tcp_server_handle_async_uv_write);
  if (uv_result < 0) {
    uv_result_ptr[0] = uv_result;
    free(async_write_handle);
    return CARLIE_TCP_SERVER_RESULT_UV_FAILURE;
  }
  carlie_tcp_server_async_uv_write_data_t *const async_write_data = malloc(sizeof(carlie_tcp_server_async_uv_write_data_t));
  if (async_write_data == null_ptr) {
    uv_result_ptr[0] = 0;
    free(async_write_handle);
    return CARLIE_TCP_SERVER_RESULT_MEMORY_ALLOCATION_FAILED;
  }
  uv_buf_t *const buffer = malloc(sizeof(uv_buf_t));
  if (buffer == null_ptr) {
    uv_result_ptr[0] = 0;
    free(async_write_data);
    free(async_write_handle);
    return CARLIE_TCP_SERVER_RESULT_MEMORY_ALLOCATION_FAILED;
  }
  carlie_get_array_bytes(environment, buffer_bytes, (uint8_t **) &buffer->base);
  buffer->len = buffer_bytes_size;
  buffer_bytes = (jni_byte_array_t) environment[0]->NewGlobalRef(environment, (jni_object_t) buffer_bytes);
  if (buffer_bytes == null_ptr) {
    carlie_throw_null_pointer_exception(environment, native_object->server_native_object->null_pointer_exception_class, native_object->server_native_object->null_pointer_exception_constructor_method_id);
    uv_result_ptr[0] = 0;
    carlie_release_array_bytes(environment, (uint8_t *) buffer->base, buffer_bytes, (int32_t) JNI_ABORT);
    free(buffer);
    free(async_write_data);
    free(async_write_handle);
    return CARLIE_TCP_SERVER_RESULT_NULL_POINTER_EXCEPTION_THROWN;
  }
  callback_function_object = environment[0]->NewGlobalRef(environment, callback_function_object);
  if (callback_function_object == null_ptr) {
    carlie_throw_null_pointer_exception(environment, native_object->server_native_object->null_pointer_exception_class, native_object->server_native_object->null_pointer_exception_constructor_method_id);
    uv_result_ptr[0] = 0;
    carlie_release_array_bytes(environment, (uint8_t *) buffer->base, buffer_bytes, (int32_t) JNI_ABORT);
    environment[0]->DeleteGlobalRef(environment, (jni_object_t) buffer_bytes);
    free(buffer);
    free(async_write_data);
    free(async_write_handle);
    return CARLIE_TCP_SERVER_RESULT_NULL_POINTER_EXCEPTION_THROWN;
  }
  async_write_data->buffer = buffer;
  async_write_data->buffer_array = buffer_bytes;
  async_write_data->callback_function_invoke_method_id = callback_function_invoke_method_id;
  async_write_data->callback_function_object = callback_function_object;
  async_write_data->native_object = native_object;
  uv_handle_set_data((uv_handle_t *) async_write_handle, (void *) async_write_data);
  uv_result = (int32_t) uv_async_send(async_write_handle);
  if (uv_result < 0) {
    uv_result_ptr[0] = uv_result;
    environment[0]->DeleteGlobalRef(environment, callback_function_object);
    carlie_release_array_bytes(environment, (uint8_t *) buffer->base, buffer_bytes, (int32_t) JNI_ABORT);
    environment[0]->DeleteGlobalRef(environment, (jni_object_t) buffer_bytes);
    free(buffer);
    free(async_write_data);
    free(async_write_handle);
    return CARLIE_TCP_SERVER_RESULT_UV_FAILURE;
  }
  uv_result_ptr[0] = 0;
  return CARLIE_TCP_SERVER_RESULT_SUCCESS;
}



CARLIE_C_ALWAYS_INLINE static inline carlie_tcp_server_result_t
carlie_tcp_server_connection_emit_uv_error_event(jni_environment_handle_t const environment,
                                                 carlie_tcp_server_connection_native_object_t *const native_object,
                                                 int32_t const error_number)
{
  jni_object_t exception_object = null_ptr;
  carlie_tcp_server_result_t const carlie_result = carlie_tcp_server_create_uv_exception(environment, native_object->server_native_object, error_number, &exception_object);
  if (carlie_result != CARLIE_TCP_SERVER_RESULT_SUCCESS) {
    return carlie_result;
  }
  environment[0]->CallVoidMethod(environment, native_object->handle_error_occurred_event_function_object, native_object->handle_error_occurred_event_function_handle_method_id, exception_object);
  return CARLIE_TCP_SERVER_RESULT_SUCCESS;
}



CARLIE_C_ALWAYS_INLINE static inline carlie_tcp_server_result_t
carlie_tcp_server_create_connection_native_object(jni_environment_handle_t const environment,
                                                  carlie_tcp_server_native_object_t *const server_native_object,
                                                  jni_object_t *const connection_native_object_bytes_ptr,
                                                  carlie_tcp_server_connection_native_object_t **const connection_native_object_ptr)
{
  jni_object_t const connection_native_object_bytes = environment[0]->CallObjectMethod(environment, server_native_object->create_connection_native_object_static_method_function_object, server_native_object->create_connection_native_object_static_method_function_invoke_method_id);
  carlie_get_native_object(environment, connection_native_object_bytes, (void **) connection_native_object_ptr);
  carlie_tcp_server_connection_native_object_t *const connection_native_object = connection_native_object_ptr[0];
  connection_native_object->close_flag_mutex = &connection_native_object->close_flag_mutex_;
  connection_native_object->tcp_handle = &connection_native_object->tcp_handle_;
  connection_native_object_bytes_ptr[0] = connection_native_object_bytes;
  return CARLIE_TCP_SERVER_RESULT_SUCCESS;
}



CARLIE_C_ALWAYS_INLINE static inline carlie_tcp_server_result_t
carlie_tcp_server_create_uv_exception(jni_environment_handle_t const environment,
                                      carlie_tcp_server_native_object_t *const native_object,
                                      int32_t const error_number,
                                      jni_object_t *const exception_object_ptr)
{
  jni_object_t exception_object = environment[0]->NewObject(environment, native_object->uv_exception_class, native_object->uv_exception_constructor_method_id, (jni_int_t) error_number);
  if (exception_object == null_ptr) {
    exception_object_ptr[0] = null_ptr;
    return CARLIE_TCP_SERVER_RESULT_CANNOT_CREATE_EXCEPTION;
  }
  exception_object_ptr[0] = exception_object;
  return CARLIE_TCP_SERVER_RESULT_SUCCESS;
}



CARLIE_C_ALWAYS_INLINE static inline carlie_tcp_server_result_t
carlie_tcp_server_emit_uv_error_event(jni_environment_handle_t const environment,
                                      carlie_tcp_server_native_object_t *const native_object,
                                      int32_t const error_number)
{
  jni_object_t exception_object = null_ptr;
  carlie_tcp_server_result_t const carlie_result = carlie_tcp_server_create_uv_exception(environment, native_object, error_number, &exception_object);
  if (carlie_result != CARLIE_TCP_SERVER_RESULT_SUCCESS) {
    return carlie_result;
  }
  environment[0]->CallVoidMethod(environment, native_object->handle_error_occurred_event_function_object, native_object->handle_error_occurred_event_function_handle_method_id, exception_object);
  return CARLIE_TCP_SERVER_RESULT_SUCCESS;
}



CARLIE_C_ALWAYS_INLINE static inline carlie_tcp_server_result_t
carlie_tcp_server_get_jni_environment_from_server_native_object(carlie_tcp_server_native_object_t *const native_object,
                                                                jni_environment_handle_t *const environment_ptr)
{
  jni_java_vm_t *const java_vm = native_object->java_vm;
  // NOTE: Due to how `native_object`’s data is managed, it shouldn’t be
  // possible for it to be non-null and have a null `java_vm` member.
  assert(java_vm != null_ptr);
  int32_t jni_result = (int32_t) java_vm[0]->GetEnv(java_vm, (void **) environment_ptr, JNI_VERSION);
  if (jni_result != JNI_OK) {
    // TODO: Investigate the ins and outs of how this works, thoroughly.
    jni_result = (int32_t) java_vm[0]->AttachCurrentThread(java_vm, (void **) environment_ptr, null_ptr);
    if (jni_result != JNI_OK) {
      environment_ptr[0] = null_ptr;
      return CARLIE_TCP_SERVER_RESULT_CANNOT_RETRIEVE_JNI_ENVIRONMENT;
    }
  }
  return CARLIE_TCP_SERVER_RESULT_SUCCESS;
}



CARLIE_C_ALWAYS_INLINE static inline carlie_tcp_server_result_t
carlie_tcp_server_get_uv_address(jni_environment_handle_t const environment,
                                 carlie_tcp_server_native_object_t *const native_object,
                                 uv_tcp_t *const tcp_handle,
                                 int (*const uv_get_address_function)(uv_tcp_t const *const tcp_handle,
                                                                      struct sockaddr *const socket_address_ptr,
                                                                      int *const socket_address_size_ptr),
                                 jni_object_t const create_address_method_function_object,
                                 jni_class_t const create_address_method_function_class,
                                 jni_object_t *const address_object_ptr)
{
  jni_method_id_t const create_address_method_function_invoke_method_id = environment[0]->GetMethodID(environment, create_address_method_function_class, "invoke", "(Ljava/lang/Object;Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
  if (create_address_method_function_invoke_method_id == null_ptr) {
    carlie_throw_null_pointer_exception(environment, native_object->null_pointer_exception_class, native_object->null_pointer_exception_constructor_method_id);
    return CARLIE_TCP_SERVER_RESULT_NULL_POINTER_EXCEPTION_THROWN;
  }
  struct sockaddr_storage socket_address;
  size_t socket_address_size = sizeof(socket_address);
  int32_t uv_result;
  uv_result = (int32_t) uv_get_address_function(tcp_handle, (struct sockaddr *) &socket_address, (int *) &socket_address_size);
  if (uv_result < 0) {
    carlie_tcp_server_throw_uv_exception(environment, native_object, uv_result);
    return CARLIE_TCP_SERVER_RESULT_UV_FAILURE;
  }
  char address_ip[INET6_ADDRSTRLEN];
  uint16_t address_port;
  uint8_t address_version;
  if (socket_address.ss_family == AF_INET) {
    struct sockaddr_in const *const socket_address_v4_ptr = (struct sockaddr_in *) &socket_address;
    uv_result = (int32_t) uv_ip4_name(socket_address_v4_ptr, address_ip, (size_t) INET6_ADDRSTRLEN);
    if (uv_result < 0) {
      carlie_tcp_server_throw_uv_exception(environment, native_object, uv_result);
      return CARLIE_TCP_SERVER_RESULT_UV_FAILURE;
    }
    address_port = ntohs(socket_address_v4_ptr->sin_port);
    address_version = 4u;
  } else if (socket_address.ss_family == AF_INET6) {
    struct sockaddr_in6 const *const socket_address_v6_ptr = (struct sockaddr_in6 *) &socket_address;
    uv_result = (int32_t) uv_ip6_name(socket_address_v6_ptr, address_ip, (size_t) INET6_ADDRSTRLEN);
    if (uv_result < 0) {
      carlie_tcp_server_throw_uv_exception(environment, native_object, uv_result);
      return CARLIE_TCP_SERVER_RESULT_UV_FAILURE;
    }
    address_port = ntohs(socket_address_v6_ptr->sin6_port);
    address_version = 6u;
  } else {
    carlie_throw_runtime_exception(environment, native_object->runtime_exception_class, native_object->runtime_exception_constructor_method_id);
    return CARLIE_TCP_SERVER_RESULT_RUNTIME_EXCEPTION_THROWN;
  }
  jni_string_t const address_ip_string = environment[0]->NewStringUTF(environment, address_ip);
  if (address_ip_string == null_ptr) {
    carlie_throw_null_pointer_exception(environment, native_object->null_pointer_exception_class, native_object->null_pointer_exception_constructor_method_id);
    return CARLIE_TCP_SERVER_RESULT_NULL_POINTER_EXCEPTION_THROWN;
  }
  jni_object_t const address_port_object = environment[0]->NewObject(environment, native_object->integer_class, native_object->integer_constructor_method_id, (jni_int_t) (int32_t) address_port);
  jni_object_t const address_version_object = environment[0]->NewObject(environment, native_object->integer_class, native_object->integer_constructor_method_id, (jni_int_t) (int32_t) address_version);
  if ((address_port_object == null_ptr) ||
      (address_version_object == null_ptr)) {
    carlie_throw_null_pointer_exception(environment, native_object->null_pointer_exception_class, native_object->null_pointer_exception_constructor_method_id);
    return CARLIE_TCP_SERVER_RESULT_NULL_POINTER_EXCEPTION_THROWN;
  }
  address_object_ptr[0] = environment[0]->CallObjectMethod(environment, create_address_method_function_object, create_address_method_function_invoke_method_id, address_ip_string, address_version_object, address_port_object);
  return CARLIE_TCP_SERVER_RESULT_SUCCESS;
}



CARLIE_C_ALWAYS_INLINE static inline char const *
carlie_tcp_server_result_get_value(carlie_tcp_server_result_t const status)
{
  switch (status) {
    case CARLIE_TCP_SERVER_RESULT_CANNOT_CREATE_EXCEPTION:
    case CARLIE_TCP_SERVER_RESULT_CANNOT_THROW_EXCEPTION:
    case CARLIE_TCP_SERVER_RESULT_NULL_POINTER_EXCEPTION_THROWN:
    case CARLIE_TCP_SERVER_RESULT_RUNTIME_EXCEPTION_THROWN:
    case CARLIE_TCP_SERVER_RESULT_SUCCESS: {
      return carlie_result_get_value((carlie_result_t) (unsigned int) status);
    }
    case CARLIE_TCP_SERVER_RESULT_CANNOT_RETRIEVE_JNI_ENVIRONMENT: {
      return "JNI environment retrieval failed.";
    }
    case CARLIE_TCP_SERVER_RESULT_MEMORY_ALLOCATION_FAILED: {
      return "Memory allocation failed.";
    }
    case CARLIE_TCP_SERVER_RESULT_UV_FAILURE: {
      return "libuv failure.";
    }
    default: {
      return carlie_result_get_value((carlie_result_t) (unsigned int) status);
    }
  }
}



CARLIE_C_ALWAYS_INLINE static inline carlie_tcp_server_result_t
carlie_tcp_server_throw_uv_exception(jni_environment_handle_t const environment,
                                     carlie_tcp_server_native_object_t *const native_object,
                                     int32_t const error_number)
{
  jni_object_t exception_object = null_ptr;
  carlie_tcp_server_result_t const carlie_result = carlie_tcp_server_create_uv_exception(environment, native_object, error_number, &exception_object);
  if (carlie_result != CARLIE_TCP_SERVER_RESULT_SUCCESS) {
    carlie_throw_null_pointer_exception(environment, native_object->null_pointer_exception_class, native_object->null_pointer_exception_constructor_method_id);
    return CARLIE_TCP_SERVER_RESULT_NULL_POINTER_EXCEPTION_THROWN;
  }
  int32_t const jni_result = (int32_t) environment[0]->Throw(environment, (jni_throwable_t) exception_object);
  if (jni_result < 0) {
    // When shit hits the fan!
    carlie_throw_runtime_exception(environment, native_object->runtime_exception_class, native_object->runtime_exception_constructor_method_id);
    return CARLIE_TCP_SERVER_RESULT_RUNTIME_EXCEPTION_THROWN;
  }
  return CARLIE_TCP_SERVER_RESULT_SUCCESS;
}



/*
 *******************************************************************************
 * Class:     io_seventeenninetyone_carlie_TcpServer                           *
 * Method:    getConnectionNativeObjectSize                                    *
 * Signature: ()I                                                              *
 *******************************************************************************
 */
JNI_DEFINE_METHOD(jni_int_t, getConnectionNativeObjectSize)(jni_environment_handle_t environment,
                                                            jni_class_t server_class);



/*
 *******************************************************************************
 * Class:     io_seventeenninetyone_carlie_TcpServer                           *
 * Method:    getNativeObjectSize                                              *
 * Signature: ()I                                                              *
 *******************************************************************************
 */
JNI_DEFINE_METHOD(jni_int_t, getNativeObjectSize)(jni_environment_handle_t environment,
                                                  jni_class_t server_class);



/*
 *******************************************************************************
 * Class:     io_seventeenninetyone_carlie_TcpServer                           *
 * Method:    initializeNative                                                 *
 * Signature: (Ljava/nio/ByteBuffer;                                           *
 *             Lkotlin/jvm/functions/Function0;                                *
 *             Ljava/lang/Class;                                               *
 *             Lkotlin/jvm/functions/Function1;                                *
 *             Ljava/lang/Class;                                               *
 *             Lio/seventeenninetyone/carlie/tcp_server/ClosedEventHandlerFunction; *
 *             Ljava/lang/Class;                                               *
 *             Lio/seventeenninetyone/carlie/tcp_server/ClientConnectedEventHandlerFunction; *
 *             Ljava/lang/Class;                                               *
 *             Lio/seventeenninetyone/carlie/tcp_server/ErrorOccurredEventHandlerFunction; *
 *             Ljava/lang/Class;                                               *
 *             Ljava/lang/Class;                                               *
 *             Ljava/lang/Class;                                               *
 *             Ljava/lang/Class;                                               *
 *             Ljava/lang/Class;)Z                                             *
 *******************************************************************************
 */
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
                                                   jni_class_t uv_exception_class);



/*
 *******************************************************************************
 * Class:     io_seventeenninetyone_carlie_TcpServer                           *
 * Method:    closeNative                                                      *
 * Signature: (Ljava/nio/ByteBuffer;)V                                         *
 *******************************************************************************
 */
JNI_DEFINE_METHOD(void, closeNative)(jni_environment_handle_t environment,
                                     jni_object_t server_object,
                                     jni_object_t native_object_bytes);



/*
 *******************************************************************************
 * Class:     io_seventeenninetyone_carlie_TcpServer                           *
 * Method:    bindUvTcpHandle                                                  *
 * Signature: (Ljava/nio/ByteBuffer;                                           *
 *             Ljava/lang/String;                                              *
 *             B                                                               *
 *             I)V                                                             *
 *******************************************************************************
 */
JNI_DEFINE_METHOD(void, bindUvTcpHandle)(jni_environment_handle_t environment,
                                         jni_object_t server_object,
                                         jni_object_t native_object_bytes,
                                         jni_string_t ip_address_string,
                                         jni_byte_t ip_address_type,
                                         jni_int_t port);



/*
 *******************************************************************************
 * Class:     io_seventeenninetyone_carlie_TcpServer                           *
 * Method:    closeUvTcpHandle                                                 *
 * Signature: (Ljava/nio/ByteBuffer;)V                                         *
 *******************************************************************************
 */
JNI_DEFINE_METHOD(void, closeUvTcpHandle)(jni_environment_handle_t environment,
                                          jni_object_t server_object,
                                          jni_object_t native_object_bytes);



/*
 *******************************************************************************
 * Class:     io_seventeenninetyone_carlie_TcpServer                           *
 * Method:    getUvTcpBoundAddress                                             *
 * Signature: (Ljava/nio/ByteBuffer;                                           *
 *             Lkotlin/jvm/functions/Function3;                                *
 *             Ljava/lang/Class;)Lio/seventeenninetyone/carlie/TcpServer$Address; *
 *******************************************************************************
 */
JNI_DEFINE_METHOD(jni_object_t, getUvTcpBoundAddress)(jni_environment_handle_t environment,
                                                      jni_object_t server_object,
                                                      jni_object_t native_object_bytes,
                                                      jni_object_t create_address_method_function_object,
                                                      jni_class_t create_address_method_function_class);



/*
 *******************************************************************************
 * Class:     io_seventeenninetyone_carlie_TcpServer                           *
 * Method:    initializeUvTcpHandle                                            *
 * Signature: (Ljava/nio/ByteBuffer;)V                                         *
 *******************************************************************************
 */
JNI_DEFINE_METHOD(void, initializeUvTcpHandle)(jni_environment_handle_t environment,
                                               jni_object_t server_object,
                                               jni_object_t native_object_bytes);



/*
 *******************************************************************************
 * Class:     io_seventeenninetyone_carlie_TcpServer                           *
 * Method:    uvRun                                                            *
 * Signature: (Ljava/nio/ByteBuffer;)V                                         *
 *******************************************************************************
 */
JNI_DEFINE_METHOD(void, uvRun)(jni_environment_handle_t environment,
                               jni_object_t server_object,
                               jni_object_t native_object_bytes);



/*
 *******************************************************************************
 * Class:     io_seventeenninetyone_carlie_TcpServer                           *
 * Method:    uvTcpListen                                                      *
 * Signature: (Ljava/nio/ByteBuffer;)V                                         *
 *******************************************************************************
 */
JNI_DEFINE_METHOD(void, uvTcpListen)(jni_environment_handle_t environment,
                                     jni_object_t server_object,
                                     jni_object_t native_object_bytes);



/*
 *******************************************************************************
 * Class:     io_seventeenninetyone_carlie_TcpServer_ConnectionInternal        *
 * Method:    initializeNative                                                 *
 * Signature: (Ljava/nio/ByteBuffer;                                           *
 *             Ljava/nio/ByteBuffer;                                           *
 *             Lkotlin/jvm/functions/Function0;                                *
 *             Ljava/lang/Class;                                               *
 *             Lio/seventeenninetyone/carlie/tcp_server/ClosedEventHandlerFunction; *
 *             Ljava/lang/Class;                                               *
 *             Lio/seventeenninetyone/carlie/tcp_server/ErrorOccurredEventHandlerFunction; *
 *             Ljava/lang/Class;)Z                                             *
 *******************************************************************************
 */
JNI_DEFINE_CONNECTION_INTERNAL_METHOD(jni_boolean_t, initializeNative)(jni_environment_handle_t environment,
                                                                       jni_object_t connection_object,
                                                                       jni_object_t connection_native_object_bytes,
                                                                       jni_object_t server_native_object_bytes,
                                                                       jni_object_t close_method_function_object,
                                                                       jni_class_t close_method_function_class,
                                                                       jni_object_t handle_closed_event_function_object,
                                                                       jni_class_t handle_closed_event_function_class,
                                                                       jni_object_t handle_error_occurred_event_function_object,
                                                                       jni_class_t handle_error_occurred_event_function_class);



/*
 *******************************************************************************
 * Class:     io_seventeenninetyone_carlie_TcpServer_ConnectionInternal        *
 * Method:    closeNative                                                      *
 * Signature: (Ljava/nio/ByteBuffer;)V                                         *
 *******************************************************************************
 */
JNI_DEFINE_CONNECTION_INTERNAL_METHOD(void, closeNative)(jni_environment_handle_t environment,
                                                         jni_object_t connection_object,
                                                         jni_object_t native_object_bytes);



/*
 *******************************************************************************
 * Class:     io_seventeenninetyone_carlie_TcpServer_ConnectionInternal        *
 * Method:    closeInUvWorker                                                  *
 * Signature: (Ljava/nio/ByteBuffer;)V                                         *
 *******************************************************************************
 */
JNI_DEFINE_CONNECTION_INTERNAL_METHOD(void, closeInUvWorker)(jni_environment_handle_t environment,
                                                             jni_object_t connection_object,
                                                             jni_object_t native_object_bytes);



/*
 *******************************************************************************
 * Class:     io_seventeenninetyone_carlie_TcpServer_ConnectionInternal        *
 * Method:    closeUvTcpHandle                                                 *
 * Signature: (Ljava/nio/ByteBuffer;)V                                         *
 *******************************************************************************
 */
JNI_DEFINE_CONNECTION_INTERNAL_METHOD(void, closeUvTcpHandle)(jni_environment_handle_t environment,
                                                              jni_object_t connection_object,
                                                              jni_object_t native_object_bytes);



/*
 *******************************************************************************
 * Class:     io_seventeenninetyone_carlie_TcpServer_ConnectionInternal        *
 * Method:    getUvTcpBoundAddress                                             *
 * Signature: (Ljava/nio/ByteBuffer;                                           *
 *             Lkotlin/jvm/functions/Function3;                                *
 *             Ljava/lang/Class;)Lio/seventeenninetyone/carlie/TcpServer$Address; *
 *******************************************************************************
 */
JNI_DEFINE_CONNECTION_INTERNAL_METHOD(jni_object_t, getUvTcpBoundAddress)(jni_environment_handle_t environment,
                                                                          jni_object_t connection_object,
                                                                          jni_object_t native_object_bytes,
                                                                          jni_object_t create_address_method_function_object,
                                                                          jni_class_t create_address_method_function_class);



/*
 *******************************************************************************
 * Class:     io_seventeenninetyone_carlie_TcpServer_ConnectionInternal        *
 * Method:    getUvTcpRemoteAddress                                            *
 * Signature: (Ljava/nio/ByteBuffer;                                           *
 *             Lkotlin/jvm/functions/Function3;                                *
 *             Ljava/lang/Class;)Lio/seventeenninetyone/carlie/TcpServer$Address; *
 *******************************************************************************
 */
JNI_DEFINE_CONNECTION_INTERNAL_METHOD(jni_object_t, getUvTcpRemoteAddress)(jni_environment_handle_t environment,
                                                                           jni_object_t connection_object,
                                                                           jni_object_t native_object_bytes,
                                                                           jni_object_t create_address_method_function_object,
                                                                           jni_class_t create_address_method_function_class);



/*
 *******************************************************************************
 * Class:     io_seventeenninetyone_carlie_TcpServer_ConnectionInternal        *
 * Method:    initializeUvTcpHandle                                            *
 * Signature: (Ljava/nio/ByteBuffer;)V                                         *
 *******************************************************************************
 */
JNI_DEFINE_CONNECTION_INTERNAL_METHOD(void, initializeUvTcpHandle)(jni_environment_handle_t environment,
                                                                   jni_object_t connection_object,
                                                                   jni_object_t native_object_bytes);



/*
 *******************************************************************************
 * Class:     io_seventeenninetyone_carlie_TcpServer_ConnectionInternal        *
 * Method:    isCloseable                                                      *
 * Signature: (Ljava/nio/ByteBuffer;)Z                                         *
 *******************************************************************************
 */
JNI_DEFINE_CONNECTION_INTERNAL_METHOD(jni_boolean_t, isCloseable)(jni_environment_handle_t environment,
                                                                  jni_object_t connection_object,
                                                                  jni_object_t native_object_bytes);



/*
 *******************************************************************************
 * Class:     io_seventeenninetyone_carlie_TcpServer_ConnectionInternal        *
 * Method:    uvTcpDisableKeepAlive                                            *
 * Signature: (Ljava/nio/ByteBuffer;)V                                         *
 *******************************************************************************
 */
JNI_DEFINE_CONNECTION_INTERNAL_METHOD(void, uvTcpDisableKeepAlive)(jni_environment_handle_t environment,
                                                                   jni_object_t connection_object,
                                                                   jni_object_t native_object_bytes);



/*
 *******************************************************************************
 * Class:     io_seventeenninetyone_carlie_TcpServer_ConnectionInternal        *
 * Method:    uvTcpEnableKeepAlive                                             *
 * Signature: (Ljava/nio/ByteBuffer;                                           *
 *             I)V                                                             *
 *******************************************************************************
 */
JNI_DEFINE_CONNECTION_INTERNAL_METHOD(void, uvTcpEnableKeepAlive)(jni_environment_handle_t environment,
                                                                  jni_object_t connection_object,
                                                                  jni_object_t native_object_bytes,
                                                                  jni_int_t initial_delay);



/*
 *******************************************************************************
 * Class:     io_seventeenninetyone_carlie_TcpServer_ConnectionInternal        *
 * Method:    uvTcpRead                                                        *
 * Signature: (Ljava/nio/ByteBuffer;                                           *
 *             [B                                                              *
 *             I                                                               *
 *             Lkotlin/jvm/functions/Function2;                                *
 *             Ljava/lang/Class;)V                                             *
 *******************************************************************************
 */
JNI_DEFINE_CONNECTION_INTERNAL_METHOD(void, uvTcpRead)(jni_environment_handle_t environment,
                                                       jni_object_t connection_object,
                                                       jni_object_t native_object_bytes,
                                                       jni_byte_array_t buffer_bytes,
                                                       jni_int_t buffer_bytes_size,
                                                       jni_object_t callback_function_object,
                                                       jni_class_t callback_function_class);



/*
 *******************************************************************************
 * Class:     io_seventeenninetyone_carlie_TcpServer_ConnectionInternal        *
 * Method:    uvTcpWrite                                                       *
 * Signature: (Ljava/nio/ByteBuffer;                                           *
 *             [B                                                              *
 *             I                                                               *
 *             Lkotlin/jvm/functions/Function2;                                *
 *             Ljava/lang/Class;)V                                             *
 *******************************************************************************
 */
JNI_DEFINE_CONNECTION_INTERNAL_METHOD(void, uvTcpWrite)(jni_environment_handle_t environment,
                                                        jni_object_t connection_object,
                                                        jni_object_t native_object_bytes,
                                                        jni_byte_array_t buffer_bytes,
                                                        jni_int_t buffer_bytes_size,
                                                        jni_object_t callback_function_object,
                                                        jni_class_t callback_function_class);



#endif
