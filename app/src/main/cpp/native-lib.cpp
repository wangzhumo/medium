#include <jni.h>
#include <string>

extern "C" {
#include "libavutil/avutil.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include <android/native_window_jni.h>
#include <android/native_window.h>
}

#ifdef ANDROID
#include <android/log.h>

#define TAG "medium-jni" // 这个是自定义的LOG的标识
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // 定义LOGD类型
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__) // 定义LOGI类型
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__) // 定义LOGW类型
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__) // 定义LOGE类型
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,TAG ,__VA_ARGS__) // 定义LOGF类型

#endif




extern "C"
JNIEXPORT jstring JNICALL
Java_com_wangzhumo_app_medium_MainActivity_stringFromJNI(JNIEnv *env, jobject thiz) {
    return env->NewStringUTF(av_version_info());
}


extern "C"
JNIEXPORT void JNICALL
Java_com_wangzhumo_app_medium_widget_CustomSurfacePlayer_start(JNIEnv *env, jobject thiz,
                                                               jstring file_path, jobject surface) {
    const char *filePath = env->GetStringUTFChars(file_path, NULL);
    int ret = -1;

    LOGD("File path = %s",filePath);
    // start ffmpeg.
    // init network
    avformat_network_init();
    // decode media

    //1.alloc ctx
    AVFormatContext *pAvFormatCtx = avformat_alloc_context();   // alloc ctx
    AVDictionary *pAvDictionary = NULL;  // NULL


    //2.config prams
    // If *pAvDictionary is NULL
    // dictionary struct is allocated and put in *pm.
    av_dict_set(&pAvDictionary, "timeout", "3000000", 0);

    //3.open media file
    ret = avformat_open_input(&pAvFormatCtx, filePath, NULL, &pAvDictionary);
    if (ret) {
        LOGE("avformat_open_input ret = %d",ret);
        LOGE("avformat_open_input error = %s",av_err2str(ret));
        return;
    }

    //4.find video strack
    ret = avformat_find_stream_info(pAvFormatCtx, NULL);
    if (ret < 0){
        LOGE("Couldn't find stream information.");
        return;
    }

    int video_stream_index = -1;
    for (int i = 0; i < pAvFormatCtx->max_streams; i++) {
        if (pAvFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && video_stream_index < 0) {
            video_stream_index = i;
            break;
        }
    }

    if (video_stream_index == -1) {
        LOGE("Didn't find a video stream or audio steam.");
        return ; // Didn't find a video stream
    }
    LOGD("find a video stream %d",video_stream_index);

    //5.get video params
    AVCodecParameters *pVideoCodecParam = pAvFormatCtx->streams[video_stream_index]->codecpar;

    //6.get decodec & codec ctx
    AVCodec *pCodec = avcodec_find_decoder(pVideoCodecParam->codec_id);
    if (pCodec == nullptr) {
        LOGE("Couldn't find Codec.\n");
        return;
    }
    LOGD("get a video decoder");

    AVCodecContext *pCodecCtx = avcodec_alloc_context3(pCodec);
    //6.1 set params to codec ctx
    avcodec_parameters_to_context(pCodecCtx, pVideoCodecParam);

    //7.open codec
    ret = avcodec_open2(pCodecCtx, pCodec, NULL);
    if (ret < 0) {
        LOGE("Could not open codec.");
        return; // Could not open codec
    }

    //8. get media packet
    //8.5 prepare swscale , for scale
    // 重视速度  fast_bilinear point
    // 重视质量  cubic,spline,lanczos
    // 缩放
    // 重视速度  fast_biliner,point
    // 重视质量  gauss,biliner
    // 重视锐度  cubic,spline,lanczos

    int destWidth = pCodecCtx->width;
    int destHeight = pCodecCtx->height;
    // 1080
//    if (destWidth > 1080){
//        // 如果它大于1080，需要把他缩小
//        float scale = 1080 / destWidth;
//        destWidth = scale * destWidth;
//        destHeight = scale * destHeight;
//    }
    SwsContext *pSwsCtx = sws_getContext(
            pCodecCtx->width,
            pCodecCtx->height,
            pCodecCtx->pix_fmt,
            destWidth,
            destHeight,
            AV_PIX_FMT_RGBA,
            SWS_BILINEAR,      // codec 中间转换的模式 - 平衡
            0, 0, 0
    );

    //8.1 alloc packet
    AVPacket *pAVPacket = av_packet_alloc();

    //9.4 设置window & 缓冲区
    // 获取native window
    LOGD("require ANativeWindow instance ");
    ANativeWindow *pNativeWindow = ANativeWindow_fromSurface(env, surface);
    if (pNativeWindow == nullptr) {
        LOGE("ANativeWindow_fromSurface == nullptr , error\n");
        return;
    }

    int32_t result = ANativeWindow_setBuffersGeometry(pNativeWindow, pCodecCtx->width, pCodecCtx->height,
                                     WINDOW_FORMAT_RGBA_8888);
    if (result != 0){
        ANativeWindow_release(pNativeWindow);
        LOGE("ANativeWindow_setBuffersGeometry error : %s ",av_err2str(result));
        return;
    }
    ANativeWindow_Buffer outBuffer;

    LOGD("start play video");
    //8.2 read frame to packet by loop
    while (av_read_frame(pAvFormatCtx, pAVPacket) >= 0) {
        //8.3 send frame to packet
        avcodec_send_packet(pCodecCtx, pAVPacket);
        //8.4 get frame from packet
        AVFrame *pAvframe = av_frame_alloc();
        ret = avcodec_receive_frame(pCodecCtx, pAvframe);
        //AVERROR(EAGAIN)  output is not available in this state
        if (ret == AVERROR(EAGAIN)) {
            // continue loop
            continue;
        } else if (ret < 0) {  //0  success, a frame was returned
            // get frame data
            break;
        }


        //9 display
        //9.1 frame data container
        // 接受的容器
        uint8_t *dst_data[4];   // 函数内部会开辟空间.赋值
        // 每一行的首地址
        int dst_line_addr[4];   // 同上

        //--------------------------
        //      r
        //--------------------------
        //      g
        //--------------------------
        //      b
        //--------------------------
        //      a
        //--------------------------

        //9.2 根据格式，准备好容器
        // align 对齐方式，左对齐
        av_image_alloc(dst_data, dst_line_addr, pCodecCtx->width, pCodecCtx->height,
                       AV_PIX_FMT_RGBA, 1);

        //9.3 缩放，转换
        sws_scale(pSwsCtx, pAvframe->data, pAvframe->linesize, 0, pAvframe->height, dst_data,
                  dst_line_addr);

        //9.4 开启锁，赋值
        ANativeWindow_lock(pNativeWindow, &outBuffer, NULL);

        uint8_t *firstWindow = static_cast<uint8_t *>(outBuffer.bits);
        //输入源
        uint8_t *src_data = dst_data[0];
        //这一行的数据长度
        int dest_length = outBuffer.stride * 4;
        int src_lenght = dst_line_addr[0];

        // 循环高度的数量，一行一行的赋值
        for (int i = 0; i < outBuffer.height; ++i) {
            // 这里 firstWindow 以及 src_data 都是指针，只要加上上一行的长度，就会移动到下一行的开始位置
            memcpy(firstWindow + (i * dest_length), src_data + (i * src_lenght), dest_length);
        }

        //9.5
        ANativeWindow_unlockAndPost(pNativeWindow);
    }

    ANativeWindow_release(pNativeWindow);
}