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



#include <jni.h>



/*
 *******************************************************************************
 * JVM and JNI-related macros.                                                 *
 *******************************************************************************
 */
#define JVM_ROOT_PACKAGE_SIGNATURE io_seventeenninetyone_carlie

#define JNI_DEFINE_METHOD1(return_type, package_signature, class_name, method_signature) \
  JNIEXPORT return_type                                                                  \
  JNICALL Java_ ## package_signature ## _ ## class_name ## _ ## method_signature
#define JNI_DEFINE_METHOD0(return_type, package_signature, class_name, method_signature) \
  JNI_DEFINE_METHOD1(return_type, package_signature, class_name, method_signature)



/*
 *******************************************************************************
 * Some useful macros.                                                         *
 *******************************************************************************
 */
#define INTERNAL_UNUSED_SYMBOL(symbol) \
  ((void) (symbol))



/*
 *******************************************************************************
 * Internal API type definitions.                                              *
 *******************************************************************************
 */
typedef jclass jni_class_t;
typedef JNIEnv * jni_environment_handle_t;



#endif
