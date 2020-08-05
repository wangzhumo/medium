cmake_minimum_required(VERSION 3.4.1)

project(medium)

# 设置FFmpeg头文件的路径
# 因为和CMakeLists.txt在同一级，所以直接写include
include_directories(include)

add_library(
        medium
        SHARED
        native-lib.cpp)

find_library(
        log-lib
        log )


# 定义一个变量avcodec
add_library(
        avcodec
        SHARED
        IMPORTED
)

#给avcodec这个变量赋值
set_target_properties(
        avcodec
        PROPERTIES
        IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/jniLibs/${ANDROID_ABI}/libavcodec.so)


# avfilter
add_library(
        avfilter
        SHARED
        IMPORTED
)
set_target_properties(
        avfilter
        PROPERTIES
        IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/jniLibs/${ANDROID_ABI}/libavfilter.so)

# avformat
add_library(
        avformat
        SHARED
        IMPORTED
)
set_target_properties(
        avformat
        PROPERTIES
        IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/jniLibs/${ANDROID_ABI}/libavformat.so)

# avutil
add_library(
        avutil
        SHARED
        IMPORTED
)
set_target_properties(
        avutil
        PROPERTIES
        IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/jniLibs/${ANDROID_ABI}/libavutil.so)

# swresample
add_library(
        swresample
        SHARED
        IMPORTED
)
set_target_properties(
        swresample
        PROPERTIES
        IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/jniLibs/${ANDROID_ABI}/libswresample.so)

# swscale
add_library(
        swscale
        SHARED
        IMPORTED
)
set_target_properties(
        swscale
        PROPERTIES
        IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/jniLibs/${ANDROID_ABI}/libswscale.so)




# 将ffmpeg混合编译到native-lib中
target_link_libraries(
        medium
        ${log-lib}
        avcodec
        avfilter
        avutil
        avformat
        swresample
        swscale
        android   #添加到编译中，这个是系统的库
        z         #ffmepg中 http 模块用到
        OpenSLES  #ffmpeg用到
)