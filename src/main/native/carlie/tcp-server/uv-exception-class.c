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



#include <carlie/tcp-server/uv-exception-class.h>



JNI_DEFINE_METHOD(jni_boolean_t, loadErrors)(jni_environment_handle_t environment,
                                             jni_class_t exception_class,
                                             jni_object_t map_object,
                                             jni_class_t map_class,
                                             jni_class_t integer_class,
                                             jni_class_t pair_class)
{
  CARLIE_INTERNAL_UNUSED_SYMBOL(exception_class);
  jni_method_id_t const map_put_method_id = environment[0]->GetMethodID(environment, map_class, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
  jni_method_id_t const integer_constructor_method_id = environment[0]->GetMethodID(environment, integer_class, "<init>", "(I)V");
  jni_method_id_t const pair_constructor_method_id = environment[0]->GetMethodID(environment, pair_class, "<init>", "(Ljava/lang/Object;Ljava/lang/Object;)V");
  if ((map_put_method_id == null_ptr) ||
      (integer_constructor_method_id == null_ptr) ||
      (pair_constructor_method_id == null_ptr)) {
    return (jni_boolean_t) false;
  }
  jni_object_t error_number_object;
  jni_string_t error_name_string;
  jni_string_t error_message_string;
  jni_object_t error_pair_object;
#define XXX(name, message)                                                                                                                         \
  error_number_object = environment[0]->NewObject(environment, integer_class, integer_constructor_method_id, (jni_int_t) (int32_t) (UV_ ## name)); \
  if (error_number_object == null_ptr) {                                                                                                           \
    return (jni_boolean_t) false;                                                                                                                  \
  }                                                                                                                                                \
  error_name_string = environment[0]->NewStringUTF(environment, (#name));                                                                          \
  error_message_string = environment[0]->NewStringUTF(environment, (message));                                                                     \
  if ((error_name_string == null_ptr) ||                                                                                                           \
      (error_message_string == null_ptr)) {                                                                                                        \
    return (jni_boolean_t) false;                                                                                                                  \
  }                                                                                                                                                \
  error_pair_object = environment[0]->NewObject(environment, pair_class, pair_constructor_method_id, error_name_string, error_message_string);     \
  if (error_pair_object == null_ptr) {                                                                                                             \
    return (jni_boolean_t) false;                                                                                                                  \
  }                                                                                                                                                \
  environment[0]->CallObjectMethod(environment, map_object, map_put_method_id, error_number_object, error_pair_object);                            \

  UV_ERRNO_MAP(XXX)
#undef XXX
  return (jni_boolean_t) true;
}
