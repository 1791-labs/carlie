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



#ifndef IO_SEVENTEENNINETYONE_CARLIE_TCP_SERVER_UV_EXCEPTION_CLASS_H
#define IO_SEVENTEENNINETYONE_CARLIE_TCP_SERVER_UV_EXCEPTION_CLASS_H 1



#include <carlie/common.h>
#include <inttypes.h>
#include <stdbool.h>
#include <uv.h>



/*
 *******************************************************************************
 * JVM and JNI-related macros.                                                 *
 *******************************************************************************
 */
#define JVM_PACKAGE_SIGNATURE1(root_part, rest_part) \
  root_part ## _ ## rest_part
#define JVM_PACKAGE_SIGNATURE0(root_part, rest_part) \
  JVM_PACKAGE_SIGNATURE1(root_part, rest_part)
#define JVM_PACKAGE_SIGNATURE JVM_PACKAGE_SIGNATURE0(JVM_ROOT_PACKAGE_SIGNATURE, tcp_1server)
#define JVM_CLASS_NAME UvException
#define JNI_DEFINE_METHOD(return_type, method_signature)                                   \
  JNI_DEFINE_METHOD0(return_type, JVM_PACKAGE_SIGNATURE, JVM_CLASS_NAME, method_signature)



/*
 *******************************************************************************
 * Class:     io_seventeenninetyone_carlie_tcp_server_UvException              *
 * Method:    loadErrors                                                       *
 * Signature: (Ljava/util/Map;                                                 *
 *             Ljava/lang/Class;                                               *
 *             Ljava/lang/Class;                                               *
 *             Ljava/lang/Class;)Z                                             *
 *******************************************************************************
 */
JNI_DEFINE_METHOD(jni_boolean_t, loadErrors)(jni_environment_handle_t environment,
                                             jni_class_t exception_class,
                                             jni_object_t map_object,
                                             jni_class_t map_class,
                                             jni_class_t integer_class,
                                             jni_class_t pair_class);



#endif
