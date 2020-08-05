#include <jni.h>
#include <string>
#include <android/native_window_jni.h>

extern "C" {
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}


extern "C"
JNIEXPORT jstring JNICALL
Java_com_wangzhumo_app_medium_MainActivity_stringFromJNI(JNIEnv *env, jobject thiz) {
    return env->NewStringUTF(av_version_info());
}


extern "C"
JNIEXPORT void JNICALL
Java_com_wangzhumo_app_medium_widget_CustomSurfacePlayer_start(JNIEnv *env, jobject thiz,
                                                               jstring file_path, jobject surface) {
    const char *filePath = env->GetStringUTFChars(file_path, 0);
    int ret = -1;


    // 创建windows
    ANativeWindow *pNativeWindow = ANativeWindow_fromSurface(env, surface);

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
        return;
    }

    //4.find video strack
    avformat_find_stream_info(pAvFormatCtx, NULL);
    int video_stream_index = -1;
    for (int i = 0; i < pAvFormatCtx->max_streams; i++) {
        if (pAvFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }

    //5.get video params
    AVCodecParameters *pVideoCodec = pAvFormatCtx->streams[video_stream_index]->codecpar;

    //6.get decodec & codec ctx
    AVCodec *pCodec = avcodec_find_decoder(pVideoCodec->codec_id);
    AVCodecContext *pCodecCtx = avcodec_alloc_context3(pCodec);
    //6.1 set params to codec ctx
    avcodec_parameters_to_context(pCodecCtx, pVideoCodec);

    //7.open codec
    avcodec_open2(pCodecCtx, pCodec, NULL);




    //8. get media packet
    //8.5 prepare swscale , for scale
    // 重视速度  fast_bilinear point
    // 重视质量  cubic,spline,lanczos
    // 缩放
    // 重视速度  fast_biliner,point
    // 重视质量  gauss,biliner
    // 重视锐度  cubic,spline,lanczos
    SwsContext *pSwsCtx = sws_getContext(
            pCodecCtx->width,
            pCodecCtx->height,
            pCodecCtx->pix_fmt,
            pCodecCtx->width,
            pCodecCtx->height,
            AV_PIX_FMT_RGBA,
            SWS_BILINEAR,      // codec 中间转换的模式 - 平衡
            0, 0, 0
    );

    //8.1 alloc packet
    AVPacket *pAVPacket = av_packet_alloc();

    //9.4 设置window的缓冲区
    ANativeWindow_setBuffersGeometry(pNativeWindow, pCodecCtx->width, pCodecCtx->height,
                                     WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer outBuffer;

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


}