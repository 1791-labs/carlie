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



#ifndef IO_SEVENTEENNINETYONE_CARLIE_COMMON_H
#define IO_SEVENTEENNINETYONE_CARLIE_COMMON_H 1



#include <assert.h>
#include <jni.h>
#include <inttypes.h>



/*
 *******************************************************************************
 * JVM and JNI-related macros.                                                 *
 *******************************************************************************
 */
#define JNI_DEFINE_METHOD1(return_type, package_signature, class_name, method_signature) \
  JNIEXPORT return_type                                                                  \
  JNICALL Java_ ## package_signature ## _ ## class_name ## _ ## method_signature
#define JNI_DEFINE_METHOD0(return_type, package_signature, class_name, method_signature) \
  JNI_DEFINE_METHOD1(return_type, package_signature, class_name, method_signature)
#define JNI_VERSION JNI_VERSION_1_8
#define JVM_ROOT_PACKAGE_SIGNATURE io_seventeenninetyone_carlie



/*
 *******************************************************************************
 * Some useful macros.                                                         *
 *******************************************************************************
 */
#if defined(__GNUC__) &&      \
    ((__GNUC__ > 3) ||        \
     ((__GNUC__ == 3) &&      \
      (__GNUC_MINOR__ >= 1)))
#define CARLIE_C_ALWAYS_INLINE __attribute__ ((always_inline))
#else
#define CARLIE_C_ALWAYS_INLINE
#endif

#define CARLIE_INTERNAL_UNUSED_SYMBOL(symbol) \
  ((void) (symbol))
#define null_ptr NULL



/*
 *******************************************************************************
 * Internal API type definitions.                                              *
 *******************************************************************************
 */
typedef jboolean jni_boolean_t;
typedef jbyte jni_byte_t;
typedef jbyteArray jni_byte_array_t;
typedef jclass jni_class_t;
typedef JNIEnv * jni_environment_handle_t;
typedef jint jni_int_t;
typedef JavaVM jni_java_vm_t;
typedef jmethodID jni_method_id_t;
typedef jobject jni_object_t;
typedef jstring jni_string_t;
typedef jthrowable jni_throwable_t;



typedef enum {
  CARLIE_RESULT_SUCCESS = 0u,
  CARLIE_RESULT_CANNOT_CREATE_EXCEPTION,
  CARLIE_RESULT_CANNOT_THROW_EXCEPTION,
  CARLIE_RESULT_NULL_POINTER_EXCEPTION_THROWN,
  CARLIE_RESULT_RUNTIME_EXCEPTION_THROWN,
} carlie_result_t;



CARLIE_C_ALWAYS_INLINE static inline carlie_result_t
carlie_get_array_bytes(jni_environment_handle_t const environment,
                       jni_byte_array_t const array,
                       uint8_t **const bytes_ptr);



CARLIE_C_ALWAYS_INLINE static inline carlie_result_t
carlie_get_native_object(jni_environment_handle_t const environment,
                         jni_object_t const native_object_bytes,
                         void **const native_object_ptr);



CARLIE_C_ALWAYS_INLINE static inline carlie_result_t
carlie_release_array_bytes(jni_environment_handle_t const environment,
                           uint8_t *const bytes,
                           jni_byte_array_t const array,
                           int32_t const mode);



CARLIE_C_ALWAYS_INLINE static inline char const *
carlie_result_get_value(carlie_result_t const status);



CARLIE_C_ALWAYS_INLINE static inline carlie_result_t
carlie_create_null_pointer_exception(jni_environment_handle_t const environment,
                                     jni_class_t const exception_class,
                                     jni_method_id_t const exception_constructor_method_id,
                                     jni_object_t *const exception_object_ptr);



CARLIE_C_ALWAYS_INLINE static inline carlie_result_t
carlie_create_runtime_exception(jni_environment_handle_t const environment,
                                jni_class_t const exception_class,
                                jni_method_id_t const exception_constructor_method_id,
                                jni_object_t *const exception_object_ptr);



CARLIE_C_ALWAYS_INLINE static inline carlie_result_t
carlie_throw_null_pointer_exception(jni_environment_handle_t const environment,
                                    jni_class_t const exception_class,
                                    jni_method_id_t const exception_constructor_method_id);



CARLIE_C_ALWAYS_INLINE static inline carlie_result_t
carlie_throw_runtime_exception(jni_environment_handle_t const environment,
                               jni_class_t const exception_class,
                               jni_method_id_t const exception_constructor_method_id);



CARLIE_C_ALWAYS_INLINE static inline carlie_result_t
carlie_get_array_bytes(jni_environment_handle_t const environment,
                       jni_byte_array_t const array,
                       uint8_t **const bytes_ptr)
{
  uint8_t *const bytes = (uint8_t *) environment[0]->GetByteArrayElements(environment, array, null_ptr);
  assert(bytes != null_ptr);
  bytes_ptr[0] = bytes;
  return CARLIE_RESULT_SUCCESS;
}



CARLIE_C_ALWAYS_INLINE static inline carlie_result_t
carlie_get_native_object(jni_environment_handle_t const environment,
                         jni_object_t const native_object_bytes,
                         void **const native_object_ptr)
{
  void *const native_object = environment[0]->GetDirectBufferAddress(environment, native_object_bytes);
  assert(native_object != null_ptr);
  native_object_ptr[0] = native_object;
  return CARLIE_RESULT_SUCCESS;
}



CARLIE_C_ALWAYS_INLINE static inline carlie_result_t
carlie_release_array_bytes(jni_environment_handle_t const environment,
                           uint8_t *const bytes,
                           jni_byte_array_t const array,
                           int32_t const mode)
{
  environment[0]->ReleaseByteArrayElements(environment, array, (jni_byte_t *) bytes, (jni_int_t) mode);
  return CARLIE_RESULT_SUCCESS;
}



CARLIE_C_ALWAYS_INLINE static inline char const *
carlie_result_get_value(carlie_result_t const status)
{
  switch (status) {
    case CARLIE_RESULT_CANNOT_CREATE_EXCEPTION: {
      return "Exception object creation failed.";
    }
    case CARLIE_RESULT_CANNOT_THROW_EXCEPTION: {
      return "Exception could not be thrown.";
    }
    case CARLIE_RESULT_NULL_POINTER_EXCEPTION_THROWN: {
      return "Null-pointer exception thrown.";
    }
    case CARLIE_RESULT_RUNTIME_EXCEPTION_THROWN: {
      return "Runtime exception thrown.";
    }
    case CARLIE_RESULT_SUCCESS: {
      return "Success.";
    }
    default: {
      return "Unknown error.";
    }
  }
}



CARLIE_C_ALWAYS_INLINE static inline carlie_result_t
carlie_create_null_pointer_exception(jni_environment_handle_t const environment,
                                     jni_class_t const exception_class,
                                     jni_method_id_t const exception_constructor_method_id,
                                     jni_object_t *const exception_object_ptr)
{
  jni_object_t const exception_object = environment[0]->NewObject(environment, exception_class, exception_constructor_method_id);
  if (exception_object == null_ptr) {
    exception_object_ptr[0] = null_ptr;
    return CARLIE_RESULT_CANNOT_CREATE_EXCEPTION;
  }
  exception_object_ptr[0] = exception_object;
  return CARLIE_RESULT_SUCCESS;
}



CARLIE_C_ALWAYS_INLINE static inline carlie_result_t
carlie_create_runtime_exception(jni_environment_handle_t const environment,
                                jni_class_t const exception_class,
                                jni_method_id_t const exception_constructor_method_id,
                                jni_object_t *const exception_object_ptr)
{
  jni_object_t const exception_object = environment[0]->NewObject(environment, exception_class, exception_constructor_method_id);
  if (exception_object == null_ptr) {
    exception_object_ptr[0] = null_ptr;
    return CARLIE_RESULT_CANNOT_CREATE_EXCEPTION;
  }
  exception_object_ptr[0] = exception_object;
  return CARLIE_RESULT_SUCCESS;
}



CARLIE_C_ALWAYS_INLINE static inline carlie_result_t
carlie_throw_null_pointer_exception(jni_environment_handle_t const environment,
                                    jni_class_t const exception_class,
                                    jni_method_id_t const exception_constructor_method_id)
{
  jni_object_t exception_object;
  carlie_result_t const carlie_result = carlie_create_null_pointer_exception(environment, exception_class, exception_constructor_method_id, &exception_object);
  if (carlie_result != CARLIE_RESULT_SUCCESS) {
    return carlie_result;
  }
  int32_t const jni_result = (int32_t) environment[0]->Throw(environment, (jni_throwable_t) exception_object);
  if (jni_result < 0) {
    return CARLIE_RESULT_CANNOT_THROW_EXCEPTION;
  }
  return CARLIE_RESULT_SUCCESS;
}



CARLIE_C_ALWAYS_INLINE static inline carlie_result_t
carlie_throw_runtime_exception(jni_environment_handle_t const environment,
                               jni_class_t const exception_class,
                               jni_method_id_t const exception_constructor_method_id)
{
  jni_object_t exception_object;
  carlie_result_t const carlie_result = carlie_create_runtime_exception(environment, exception_class, exception_constructor_method_id, &exception_object);
  if (carlie_result != CARLIE_RESULT_SUCCESS) {
    return carlie_result;
  }
  int32_t const jni_result = (int32_t) environment[0]->Throw(environment, (jni_throwable_t) exception_object);
  if (jni_result < 0) {
    return CARLIE_RESULT_CANNOT_THROW_EXCEPTION;
  }
  return CARLIE_RESULT_SUCCESS;
}



#endif
