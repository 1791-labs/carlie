################################################################################
# Copyright 2019-present Jay B. <j@1791.io>                                    #
#                                                                              #
# Licensed under the Apache License, Version 2.0 (the "License");              #
# you may not use this file except in compliance with the License.             #
# You may obtain a copy of the License at                                      #
#                                                                              #
#     http://www.apache.org/licenses/LICENSE-2.0                               #
#                                                                              #
# Unless required by applicable law or agreed to in writing, software          #
# distributed under the License is distributed on an "AS IS" BASIS,            #
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.     #
# See the License for the specific language governing permissions and          #
# limitations under the License.                                               #
################################################################################



################################################################################
# Require CMake v3.0+ (fail the build otherwise).                              #
################################################################################
cmake_minimum_required(
  VERSION "3.0"
  FATAL_ERROR
)



################################################################################
# Get the build target architecture and OS.                                    #
################################################################################
set(CARLIE_BUILD_TARGET_ARCHITECTURE $ENV{CARLIE_BUILD_TARGET_ARCHITECTURE})
set(CARLIE_BUILD_TARGET_OS $ENV{CARLIE_BUILD_TARGET_OS})



################################################################################
# If the build target OS isn’t supported, fail the build. Only Linux, macOS,   #
# and Windows are currently supported.                                         #
################################################################################
if(CARLIE_BUILD_TARGET_OS STREQUAL "linux")
  set(CMAKE_SYSTEM_NAME "Linux")
elseif(CARLIE_BUILD_TARGET_OS STREQUAL "mac")
  set(CMAKE_SYSTEM_NAME "Darwin")
elseif(CARLIE_BUILD_TARGET_OS STREQUAL "windows")
  set(CMAKE_SYSTEM_NAME "Windows")
else()
  message(FATAL_ERROR "Only Linux, macOS, and Windows builds are currently supported; failing the build.")
endif()



################################################################################
# Define the project (including setting its name, version, etc.).              #
# NOTE: The project’s version is expected to be passed in as an environment    #
# variable: `CARLIE_PROJECT_VERSION`.                                          #
################################################################################
set(CARLIE_PROJECT_VERSION $ENV{CARLIE_PROJECT_VERSION})

project(
  carlie
  LANGUAGES "C"
  VERSION ${CARLIE_PROJECT_VERSION}
)



################################################################################
# If the C/C++ compiler isn’t GCC or Clang (Apple or LLVM), fail the build.    #
################################################################################
if(NOT CMAKE_C_COMPILER_ID MATCHES "AppleClang|Clang|GNU")
  message(FATAL_ERROR "The C/C++ compiler must be GCC or Clang (Apple or LLVM); failing the build.")
endif()



################################################################################
# If a build type isn’t provided to CMake, default to the build type provided  #
# by the environment variable: `CARLIE_CMAKE_BUILD_TYPE`; otherwise, default   #
# to being a release build. If the build type is invalid or not supported,     #
# default to being a release build.                                            #
################################################################################
if(NOT CMAKE_BUILD_TYPE)
  set(CARLIE_CMAKE_BUILD_TYPE $ENV{CARLIE_CMAKE_BUILD_TYPE})

  if(CARLIE_CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE ${CARLIE_CMAKE_BUILD_TYPE})
  else()
    set(CMAKE_BUILD_TYPE "Release")
  endif()
endif()

if(NOT CMAKE_BUILD_TYPE MATCHES "Debug|Release")
  set(CMAKE_BUILD_TYPE "Release")
endif()



################################################################################
# Define the project’s sources.                                                #
################################################################################
set(CARLIE_PROJECT_SOURCE_FILES "")

list(APPEND CARLIE_PROJECT_SOURCE_FILES "dummy-class.c")



################################################################################
# Define the project’s [shared] library.                                       #
################################################################################
add_library(
  carlie_jni
  SHARED
  ${CARLIE_PROJECT_SOURCE_FILES}
)



################################################################################
# Tell CMake to set the compiler’s standard to C99 (potentially with GNU       #
# extensions).                                                                 #
################################################################################
macro(use_c99_c_standard)
  if(NOT CMAKE_VERSION VERSION_LESS "3.1")
    set_target_properties(carlie_jni PROPERTIES C_STANDARD "99")
  else()
    target_compile_options(
      carlie_jni
      BEFORE
      PRIVATE "-std=gnu99"
    )
  endif()
endmacro(use_c99_c_standard)

use_c99_c_standard()



################################################################################
# Create the variable that will hold the project’s include directories.        #
################################################################################
set(CARLIE_PROJECT_INCLUDE_DIRECTORIES "")



################################################################################
# Add the build target-specific native include directory for this project as   #
# an include directory for the build.                                          #
################################################################################
set(CARLIE_BUILD_JVM_NATIVE_INCLUDE_DIRECTORY $ENV{CARLIE_BUILD_JVM_NATIVE_INCLUDE_DIRECTORY})

list(APPEND CARLIE_PROJECT_INCLUDE_DIRECTORIES ${CARLIE_BUILD_JVM_NATIVE_INCLUDE_DIRECTORY})



################################################################################
# Add the build target’s JDK include directories as include directories for    #
# the build.                                                                   #
################################################################################
set(CARLIE_BUILD_JDK_INCLUDE_DIRECTORIES $ENV{CARLIE_BUILD_JDK_INCLUDE_DIRECTORIES})

list(APPEND CARLIE_PROJECT_INCLUDE_DIRECTORIES ${CARLIE_BUILD_JDK_INCLUDE_DIRECTORIES})



################################################################################
# Add the project’s parent directory as an include directory for the build.    #
# This makes it possible to include headers in C like this:                    #
# `#include <carlie/some-header.h>`. Not necessary, but convenient!            #
################################################################################
get_filename_component(
  CARLIE_PROJECT_MAIN_INCLUDE_DIRECTORY
  ${carlie_SOURCE_DIR}
  DIRECTORY
)

list(APPEND CARLIE_PROJECT_INCLUDE_DIRECTORIES ${CARLIE_PROJECT_MAIN_INCLUDE_DIRECTORY})



################################################################################
# Set the project’s include directories.                                       #
################################################################################
target_include_directories(
  carlie_jni
  BEFORE
  PRIVATE ${CARLIE_PROJECT_INCLUDE_DIRECTORIES}
)



################################################################################
# If this is a debug build, tell the compiler to generate debug information    #
# and disable as many optimizations as possible; otherwise, if this is a       #
# release build, tell the compiler to optimize like there’s no tomorrow.       #
################################################################################
set(CARLIE_PROJECT_COMPILER_OPTIONS "")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_C_FLAGS_DEBUG "")

  list(APPEND CARLIE_PROJECT_COMPILER_OPTIONS "-g"
                                              "-O0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_C_FLAGS_RELEASE "")

  if(NOT CARLIE_BUILD_TARGET_OS STREQUAL "windows")
    # LTO (link-time optimization) is, in my experience, quite buggy on Windows
    # (i.e., MinGW). Revisit this.
    list(APPEND CARLIE_PROJECT_COMPILER_OPTIONS "-flto")
  endif()

  list(APPEND CARLIE_PROJECT_COMPILER_OPTIONS "-O3")
endif()

target_compile_options(carlie_jni PRIVATE ${CARLIE_PROJECT_COMPILER_OPTIONS})



################################################################################
# Set the project’s main compiler options.                                     #
################################################################################
set(CARLIE_PROJECT_COMPILER_OPTIONS "")

if(CARLIE_BUILD_TARGET_ARCHITECTURE STREQUAL "x86")
  list(APPEND CARLIE_PROJECT_COMPILER_OPTIONS "-m32")
elseif(CARLIE_BUILD_TARGET_ARCHITECTURE STREQUAL "x86_64")
  list(APPEND CARLIE_PROJECT_COMPILER_OPTIONS "-m64")
endif()

list(APPEND CARLIE_PROJECT_COMPILER_OPTIONS "-pedantic"
                                            "-pedantic-errors"
                                            "-pthread"
                                            "-Wall"
                                            "-Werror"
                                            "-Wextra")

# if(NOT CARLIE_BUILD_TARGET_OS STREQUAL "windows")
#   list(APPEND CARLIE_PROJECT_COMPILER_OPTIONS "-fPIC"
#                                               "-fvisibility=hidden")
# endif()
set_target_properties(carlie_jni PROPERTIES C_VISIBILITY_PRESET "hidden"
                                            POSITION_INDEPENDENT_CODE ON)

target_compile_options(carlie_jni PRIVATE ${CARLIE_PROJECT_COMPILER_OPTIONS})



################################################################################
# If the build target OS is Windows, make sure that the project’s library      #
# isn’t prefixed with anything (e.g., with "lib") when built.                  #
################################################################################
if(CARLIE_BUILD_TARGET_OS STREQUAL "windows")
  set_target_properties(carlie_jni PROPERTIES PREFIX "")
endif()



################################################################################
# Create the variable that will hold the project’s library dependencies.       #
################################################################################
set(CARLIE_PROJECT_NATIVE_LIBRARY_DEPENDENCIES "")



################################################################################
# Collect the library dependencies for library dependency: uv.                 #
################################################################################
set(CARLIE_PROJECT_UV_NATIVE_LIBRARY_LIBRARY_DEPENDENCIES "")

list(APPEND CARLIE_PROJECT_UV_NATIVE_LIBRARY_LIBRARY_DEPENDENCIES "pthread")

if(CARLIE_BUILD_TARGET_OS STREQUAL "linux")
  list(APPEND CARLIE_PROJECT_UV_NATIVE_LIBRARY_LIBRARY_DEPENDENCIES "dl"
                                                                    "nsl"
                                                                    "rt")
endif()

if(CARLIE_BUILD_TARGET_OS STREQUAL "windows")
  list(APPEND CARLIE_PROJECT_UV_NATIVE_LIBRARY_LIBRARY_DEPENDENCIES "iphlpapi"
                                                                    "psapi"
                                                                    "shell32"
                                                                    "user32"
                                                                    "userenv"
                                                                    "ws2_32")
endif()



################################################################################
# Set and prepare/process the project’s library dependencies for the linker.   #
################################################################################
set(CARLIE_BUILD_JVM_NATIVE_LIBRARIES_DIRECTORY $ENV{CARLIE_BUILD_JVM_NATIVE_LIBRARIES_DIRECTORY})

list(APPEND CARLIE_PROJECT_NATIVE_LIBRARY_DEPENDENCIES "pthread")

list(APPEND CARLIE_PROJECT_NATIVE_LIBRARY_DEPENDENCIES ${CARLIE_PROJECT_UV_NATIVE_LIBRARY_LIBRARY_DEPENDENCIES}
                                                       "${CARLIE_BUILD_JVM_NATIVE_LIBRARIES_DIRECTORY}/libuv.a")

list(REMOVE_DUPLICATES CARLIE_PROJECT_NATIVE_LIBRARY_DEPENDENCIES)

set(TMP_CARLIE_PROJECT_NATIVE_LIBRARY_DEPENDENCIES "")

foreach(dependency ${CARLIE_PROJECT_NATIVE_LIBRARY_DEPENDENCIES})
  if(dependency MATCHES "\\.a$")
    list(APPEND TMP_CARLIE_PROJECT_NATIVE_LIBRARY_DEPENDENCIES ${dependency})
  else()
    list(APPEND TMP_CARLIE_PROJECT_NATIVE_LIBRARY_DEPENDENCIES "-l${dependency}")
  endif()
endforeach(dependency)

set(CARLIE_PROJECT_NATIVE_LIBRARY_DEPENDENCIES ${TMP_CARLIE_PROJECT_NATIVE_LIBRARY_DEPENDENCIES})

unset(TMP_CARLIE_PROJECT_NATIVE_LIBRARY_DEPENDENCIES)



################################################################################
# Set the project’s linker flags.                                              #
################################################################################
set(CARLIE_PROJECT_LINKER_OPTIONS "")

if(CARLIE_BUILD_TARGET_OS MATCHES "linux|windows")
  if(CARLIE_BUILD_TARGET_OS STREQUAL "windows" AND
     CARLIE_BUILD_TARGET_ARCHITECTURE STREQUAL "x86")
    list(APPEND CARLIE_PROJECT_LINKER_OPTIONS "-Wl,--kill-at")
  endif()

  list(APPEND CARLIE_PROJECT_LINKER_OPTIONS "-static-libgcc")

  list(APPEND CARLIE_PROJECT_LINKER_OPTIONS "-Wl,--whole-archive"
                                            ${CARLIE_PROJECT_NATIVE_LIBRARY_DEPENDENCIES}
                                            "-Wl,--no-whole-archive")
  # list(APPEND CARLIE_PROJECT_LINKER_OPTIONS "-Wl,--whole-archive"
  #                                           ${CARLIE_PROJECT_NATIVE_LIBRARY_DEPENDENCIES})
elseif(CARLIE_BUILD_TARGET_OS STREQUAL "mac")
  # foreach(dependency ${CARLIE_PROJECT_NATIVE_LIBRARY_DEPENDENCIES})
  #   list(APPEND CARLIE_PROJECT_LINKER_OPTIONS "-Wl,-force_load,${dependency}")
  # endforeach(dependency)
  list(APPEND CARLIE_PROJECT_LINKER_OPTIONS "-Wl,-all_load"
                                            ${CARLIE_PROJECT_NATIVE_LIBRARY_DEPENDENCIES})
endif()

target_link_libraries(carlie_jni PRIVATE ${CARLIE_PROJECT_LINKER_OPTIONS})



################################################################################
# Install the project’s library into the appropriate directory (based on the   #
# environment variable: `CARLIE_JVM_PACKAGE_NATIVE_LIBRARIES`).                #
################################################################################
set(CARLIE_JVM_PACKAGE_NATIVE_LIBRARIES $ENV{CARLIE_JVM_PACKAGE_NATIVE_LIBRARIES})

install(TARGETS carlie_jni DESTINATION ${CARLIE_JVM_PACKAGE_NATIVE_LIBRARIES})
