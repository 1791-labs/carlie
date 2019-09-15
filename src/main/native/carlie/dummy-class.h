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



#ifndef IO_SEVENTEENNINETYONE_CARLIE_DUMMY_CLASS_H
#define IO_SEVENTEENNINETYONE_CARLIE_DUMMY_CLASS_H 1



#include <carlie/common.h>
#include <stdio.h>



/*
 *******************************************************************************
 * JVM and JNI-related macros.                                                 *
 *******************************************************************************
 */
#define JVM_PACKAGE_SIGNATURE JVM_ROOT_PACKAGE_SIGNATURE
#define JVM_CLASS_NAME Dummy

#define JNI_DEFINE_METHOD(return_type, method_signature)                                   \
  JNI_DEFINE_METHOD0(return_type, JVM_PACKAGE_SIGNATURE, JVM_CLASS_NAME, method_signature)



/*
 *******************************************************************************
 * Class:     io_seventeenninetyone_carlie_Dummy                               *
 * Method:    printHelloWorld                                                  *
 * Signature: ()V                                                              *
 *******************************************************************************
 */
JNI_DEFINE_METHOD(void, printHelloWorld)(jni_environment_handle_t,
                                         jni_class_t);



#endif
