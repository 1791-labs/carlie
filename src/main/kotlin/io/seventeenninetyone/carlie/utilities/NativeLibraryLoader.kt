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

package io.seventeenninetyone.carlie.utilities

import io.seventeenninetyone.Project
import io.seventeenninetyone.carlie.utilities.native_library_loader.LibraryFileNotFoundException
import io.seventeenninetyone.carlie.utilities.native_library_loader.LibraryLoadFailureException
import io.seventeenninetyone.carlie.utilities.native_library_loader.Platform
import io.seventeenninetyone.carlie.utilities.native_library_loader.PlatformNotSupportedException
import java.io.File
import java.io.FileOutputStream
import java.io.IOException
import java.nio.channels.FileLock
import java.nio.file.Paths
import java.util.UUID

class NativeLibraryLoader {
  companion object {
    private val CLASS = NativeLibraryLoader::class.java
    private var extractedLibraryFileLock: FileLock? = null
    private val fullLibraryName: String by lazy {
      System.mapLibraryName(this.LIBRARY_NAME)
    }
    private var isLoaded = false
    private const val LIBRARY_NAME = "carlie_jni"
    private const val LIBRARY_VERSION = Project.CARLIE_VERSION
    private val libraryFileExists: Boolean by lazy {
      val resource = this.CLASS.getResource(this.libraryFilePath)
      (resource != null)
    }
    private val libraryFilePath: String by lazy {
      "${File.separator}${
        Paths.get("native-libraries", this.platformOs, this.platformArchitecture, this.fullLibraryName)
      }"
    }
    private val platformArchitecture: String by lazy {
      Platform.ARCHITECTURE!!
    }
    private val platformOs: String by lazy {
      Platform.OS!!
    }
    private val temporaryDirectoryFile: File by lazy {
      val rootDirectoryPath = System.getProperty("java.io.tmpdir")
      val temporaryDirectoryFile = File("${
        Paths.get(rootDirectoryPath, "java-io-seventeenninetyone-carlie")
      }")
      temporaryDirectoryFile.absoluteFile
    }
    private val temporaryDirectoryPath: String by lazy {
      this.temporaryDirectoryFile.absolutePath
    }

    init {
      if (! Platform.isSupported()) {
        throw PlatformNotSupportedException("The native library \"${this.LIBRARY_NAME}\" doesn’t support this platform (${Platform.sluggifiedName}) and couldn’t be loaded.")
      }
    }

    @Synchronized
    private fun cleanLeftoverMess() {
      val extractedLibraryFiles = this.temporaryDirectoryFile.listFiles {
        _, fileName ->
          val fileNameHasCorrectPrefix = fileName.startsWith("${this.LIBRARY_NAME}-${this.LIBRARY_VERSION}")
          val fileNameHasCorrectSuffix = fileName.endsWith(this.fullLibraryName)
          (fileNameHasCorrectPrefix &&
           fileNameHasCorrectSuffix)
      }
      extractedLibraryFiles?.forEach {
        file ->
          val fileOutputStream = FileOutputStream(file)
          val fileChannel = fileOutputStream!!.channel
          val fileLock = fileChannel.tryLock()
          // If an exclusive lock can be acquired, then the file isn’t locked.
          if (fileLock != null) {
            fileLock.release()
            try {
              file.delete()
            } catch (exception: IOException) {
              //
            }
          }
      }
    }

    @JvmStatic
    @Synchronized
    fun extractAndLoad() {
      if (this.isLoaded) return
      this.cleanLeftoverMess()
      if (! this.libraryFileExists) {
        throw LibraryFileNotFoundException("The library file for the native library \"${this.LIBRARY_NAME}\" wasn’t found.")
      }
      this.temporaryDirectoryFile.mkdirs()
      val extractedLibraryFile = File(this.temporaryDirectoryFile, "${this.LIBRARY_NAME}-${this.LIBRARY_VERSION}-${
        UUID.randomUUID()
      }-${this.fullLibraryName}")
      extractedLibraryFile.createNewFile()
      val extractedLibraryFileOutputStream = FileOutputStream(extractedLibraryFile)
      val extractedLibraryFileChannel = extractedLibraryFileOutputStream!!.channel
      this.extractedLibraryFileLock = extractedLibraryFileChannel.lock()
      val libraryFileInputStream = this.CLASS.getResourceAsStream(this.libraryFilePath)
      val libraryFileInputStreamBytesLength = OperatingSystem.getPageSizeOrDefault(4096u)
      val libraryFileInputStreamBytes = ByteArray(libraryFileInputStreamBytesLength.toInt())
      var libraryFileInputStreamBytesReadCount: Int
      try {
        do {
          libraryFileInputStreamBytesReadCount = libraryFileInputStream.read(libraryFileInputStreamBytes)
          if (libraryFileInputStreamBytesReadCount != -1) {
            extractedLibraryFileOutputStream.write(libraryFileInputStreamBytes, 0, libraryFileInputStreamBytesReadCount)
          }
        } while (libraryFileInputStreamBytesReadCount != -1)
      } finally {
        extractedLibraryFile.deleteOnExit()
        libraryFileInputStream.close()
      }
      // TODO: Verify/confirm that the extracted library file’s data is correct.
      extractedLibraryFile.setReadable(true)
      extractedLibraryFile.setWritable(true)
      extractedLibraryFile.setExecutable(true)
      try {
        System.load(extractedLibraryFile.absolutePath)
      } catch (exception: UnsatisfiedLinkError) {
        throw LibraryLoadFailureException("The native library \"${this.LIBRARY_NAME}\" couldn’t be loaded.")
      }
      this.isLoaded = true
    }

    protected fun finalize() {
      if (this.extractedLibraryFileLock != null) {
        val extractedLibraryChannel = this.extractedLibraryFileLock!!.acquiredBy()
        extractedLibraryChannel.close()
      }
    }
  }
}
