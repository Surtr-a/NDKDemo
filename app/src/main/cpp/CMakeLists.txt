# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# 引入头文件目录
include_directories(include)

# 把当前目录作为源文件目录赋值给 SRC_LIST
aux_source_directory(${CMAKE_SOURCE_DIR} SRC_LIST)
aux_source_directory(${CMAKE_SOURCE_DIR}/face SRC_FACE)
aux_source_directory(${CMAKE_SOURCE_DIR}/audio SRC_AUDIO)

link_directories(${CMAKE_SOURCE_DIR}/../jniLibs/arm64-v8a)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             ${SRC_LIST} ${SRC_FACE} ${SRC_AUDIO})

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                        native-lib
                        opencv_java4
                        jnigraphics
                        # 工具库（大部分库都需要这个库的支持）
                        avutil
                        # 格式封装处理
                        avformat
                        avcodec
                        # 设备信息
                        avdevice
                        # 滤镜特效处理
                        avfilter
                        # 后期处理
                        postproc
                        # 音频采样数据格式转换
                        swresample
                        # 视频像素数据格式转换
                        swscale

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )