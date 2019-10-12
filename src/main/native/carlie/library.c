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



#include <carlie/library.h>



JNIEXPORT jni_int_t
JNICALL JNI_OnLoad(jni_java_vm_t * vm,
                   void * reserved)
{
  CARLIE_INTERNAL_UNUSED_SYMBOL(vm);
  CARLIE_INTERNAL_UNUSED_SYMBOL(reserved);
  return JNI_VERSION;
}
